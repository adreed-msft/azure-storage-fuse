#include <fstream>
#include <include/permissions.h>
#include "DataLakeBfsClient.h"

///<summary>
/// Uploads contents of a file to a storage object(e.g. blob, file) to the Storage service
///</summary>
///TODO: params
///<returns>none</returns>
bool DataLakeBfsClient::AuthenticateStorage()
{
    // Authenticate the storage account
    switch (configurations.authType) {
        case KEY_AUTH:
            m_adls_client = authenticate_accountkey();
            break;
        case SAS_AUTH:
            m_adls_client = authenticate_sas();
            break;
        case MSI_AUTH:
            m_adls_client = authenticate_msi();
            break;
        default:
            return false;
            break;
    }

    if(m_adls_client != NULL)
    {
        //Authenticate the storage container by using a list call
        m_adls_client->list_paths_segmented(
                configurations.containerName,
                "/",
                1);
        if (errno != 0)
        {
            syslog(LOG_ERR,
                   "Unable to start blobfuse.  Failed to connect to the storage container. There might be something wrong about the storage config, please double check the storage account name, account key/sas token/OAuth access token and container name. errno = %d\n",
                   errno);
            return false;
        }
        return true;
    }
    return false;
}

std::shared_ptr<microsoft_azure::storage_adls::adls_client> DataLakeBfsClient::authenticate_accountkey()
{
    try
    {
        std::shared_ptr<storage_credential> cred;
        if (configurations.accountKey.length() > 0)
        {
            cred = std::make_shared<shared_key_credential>(configurations.accountName, configurations.accountKey);
        }
        else
        {
            syslog(LOG_ERR, "Empty account key. Failed to create blob client.");
            return NULL;
        }
        errno = 0;
        std::shared_ptr<storage_account> account = std::make_shared<storage_account>(
                configurations.accountName,
                cred,
                configurations.useHttps,
                configurations.blobEndpoint);
        return std::make_shared<microsoft_azure::storage_adls::adls_client>(
                account,
                max_concurrency_blob_wrapper,
                false); //If this applies to blobs in the future, we can use this as a feature to exit
                                // blobfuse if we run into anything unexpected instead of logging errors
    }
    catch(const std::exception &ex)
    {
        syslog(LOG_ERR, "Failed to create blob client.  ex.what() = %s.", ex.what());
        errno = unknown_error;
        return NULL;
    }
}
std::shared_ptr<microsoft_azure::storage_adls::adls_client> DataLakeBfsClient::authenticate_sas()
{
    try
    {
        std::shared_ptr<storage_credential> cred;
        if(configurations.sasToken.length() > 0)
        {
            cred = std::make_shared<shared_access_signature_credential>(configurations.sasToken);
        }
        else
        {
            syslog(LOG_ERR, "Empty account key. Failed to create blob client.");
            return NULL;
        }
        errno = 0;
        std::shared_ptr<storage_account> account = std::make_shared<storage_account>(
                configurations.accountName, cred,
                configurations.useHttps,
                configurations.blobEndpoint);
        return std::make_shared<microsoft_azure::storage_adls::adls_client>(
                account,
                max_concurrency_blob_wrapper,
                false); //If this applies to blobs in the future, we can use this as a feature to exit
                                // blobfuse if we run into anything unexpected instead of logging errors
    }
    catch(const std::exception &ex)
    {
        syslog(LOG_ERR, "Failed to create blob client.  ex.what() = %s.", ex.what());
        errno = unknown_error;
        return NULL;
    }
}
std::shared_ptr<microsoft_azure::storage_adls::adls_client> DataLakeBfsClient::authenticate_msi()
{
    try
    {
        //1. get oauth token
        std::function<OAuthToken(std::shared_ptr<CurlEasyClient>)> MSICallback = SetUpMSICallback(
                configurations.clientId,
                configurations.objectId,
                configurations.resourceId,
                configurations.msiEndpoint);

        std::shared_ptr<OAuthTokenCredentialManager> tokenManager = GetTokenManagerInstance(MSICallback);

        if (!tokenManager->is_valid_connection()) {
            // todo: isolate definitions of errno's for this function so we can output something meaningful.
            errno = 1;
        }

        //2. try to make blob client wrapper using oauth token
        errno = 0;
        std::shared_ptr<storage_credential> cred = std::make_shared<token_credential>();
        std::shared_ptr<storage_account> account = std::make_shared<storage_account>(
                configurations.accountName,
                cred,
                true, //use_https must be true to use oauth
                configurations.blobEndpoint);
        return std::make_shared<microsoft_azure::storage_adls::adls_client>(
                account,
                max_concurrency_blob_wrapper,
                false); //If this applies to blobs in the future, we can use this as a feature to exit
                                // blobfuse if we run into anything unexpected instead of logging errors

    }
    catch(const std::exception &ex)
    {
        syslog(LOG_ERR, "Failed to create blob client.  ex.what() = %s. Please check your account name and ", ex.what());
        errno = unknown_error;
        return NULL;
    }
}

void DataLakeBfsClient::UploadFromFile(const std::string localPath)
{
    //TODO: streams can only hold so much of the file. We should be taking it a section at a time
    //refer to blob upload from file method
    std::string dataLakeFileName = localPath.substr(configurations.tmpPath.size() + 6 /* there are six characters in "/root/" */);
    std::ifstream file_stream(localPath);
    UploadFromStream(file_stream,dataLakeFileName);
}
///<summary>
/// Uploads contents of a stream to a storage object(e.g. blob, file) to the Storage service
///</summary>
///<returns>none</returns>
void DataLakeBfsClient::UploadFromStream(std::istream & sourceStream, const std::string datalakeFilePath)
{
    m_adls_client->upload_file_from_stream(configurations.containerName, datalakeFilePath, sourceStream);
}
///<summary>
/// Downloads contents of a storage object(e.g. blob, file) to a local file
///</summary>
///<returns>none</returns>
void DataLakeBfsClient::DownloadToFile(const std::string datalakeFilePath, const std::string filePath)
{
    std::ofstream out_stream(filePath);
    m_adls_client->download_file_to_stream(configurations.containerName, datalakeFilePath, out_stream);
    out_stream.close();
}

int DataLakeBfsClient::ChangeMode(const char *path, mode_t mode) {
    // TODO: Once ADLS works in blobfuse, verify that we don't need to get the access
    microsoft_azure::storage_adls::access_control accessControl;
    accessControl.acl = modeToString(mode);

    errno = 0;
    m_adls_client->set_file_access_control(configurations.containerName, path, accessControl);

    return errno;
}