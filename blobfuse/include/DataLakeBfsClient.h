#include "StorageBfsClientBase.h"
#include "adls_client.h"
#include "BlockBlobBfsClient.h"

#ifndef DATALAKEBFSCLIENT_H
#define DATALAKEBFSCLIENT_H

class DataLakeBfsClient : public BlockBlobBfsClient
{
public:
    DataLakeBfsClient(str_options str_options) :
    BlockBlobBfsClient(str_options)
    {}
    ///<summary>
    /// Authenticates the storage account and container
    ///</summary>
    ///<returns>bool: if we authenticate to the storage account and container successfully</returns>
    bool AuthenticateStorage() override;
    ///<summary>
    /// Uploads contents of a file to a storage object(e.g. blob, file) to the Storage service
    ///</summary>
    ///TODO: params
    ///<returns>none</returns>
    void UploadFromFile(const std::string localPath) override;
    ///<summary>
    /// Uploads contents of a stream to a storage object(e.g. blob, file) to the Storage service
    ///</summary>
    ///<returns>none</returns>
    void UploadFromStream(std::istream & sourceStream, const std::string datalakeFilePath) override;
    ///<summary>
    /// Downloads contents of a storage object(e.g. blob, file) to a local file
    ///</summary>
    ///<returns>none</returns>
    void DownloadToFile(const std::string datalakeFilePath, const std::string filePath) override;
    ///<summary>
    /// Updates the UNIX-style file mode on a path.
    ///</summary>
    int ChangeMode(const char* path, mode_t mode) override;
private:
    ///<summary>
    /// Helper function - Authenticates with an account key
    ///</summary>
    std::shared_ptr<microsoft_azure::storage_adls::adls_client> authenticate_accountkey();
    ///<summary>
    /// Helper function - Authenticates with an account sas
    ///</summary>
    std::shared_ptr<microsoft_azure::storage_adls::adls_client> authenticate_sas();
    ///<summary>
    /// Helper function - Authenticates with msi
    ///</summary>
    std::shared_ptr<microsoft_azure::storage_adls::adls_client> authenticate_msi();
    ///<summary>
    /// ADLS Client to make dfs storage calls
    ///</summary>
    std::shared_ptr<microsoft_azure::storage_adls::adls_client> m_adls_client;
};

#endif //DATALAKEBFSCLIENT_H