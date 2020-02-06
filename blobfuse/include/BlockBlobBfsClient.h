#include "StorageBfsClientBase.h"
#include "blob/blob_client.h"

#ifndef BLOCKBLOBBFSCLIENTBASE_H
#define BLOCKBLOBBFSCLIENTBASE_H

class BlockBlobBfsClient : public StorageBfsClientBase
{
public:
    BlockBlobBfsClient(str_options str_options) :
    StorageBfsClientBase(str_options)
    {}

    ///<summary>
    /// Authenticates the storage account and container
    ///</summary>
    ///<returns>bool: if we authenticate to the storage account and container successfully</returns>
    bool AuthenticateStorage() override;
    ///<summary>
    /// Uploads contents of a file to a block blob to the Storage service
    ///</summary>
    ///TODO: params
    ///<returns>none</returns>
    void UploadFromFile(const std::string sourcePath) override;
    ///<summary>
    /// Uploads contents of a stream to a block blob to the Storage service
    ///</summary>
    ///<returns>none</returns>
    void UploadFromStream(std::istream & sourceStream, const std::string blobName) override;
    ///<summary>
    /// Downloads contents of a block blob to a local file
    ///</summary>
    ///<returns>none</returns>
    void DownloadToFile(const std::string blobName, const std::string filePath) override;
    ///<summary>
    /// Creates a Directory
    ///</summary>
    ///<returns>none</returns>
    bool CreateDirectory(const std::string directoryPath) override;
    ///<summary>
    /// Deletes a Directory
    ///</summary>
    ///<returns>none</returns>
    bool DeleteDirectory(const std::string directoryPath) override;
    ///<summary>
    /// Checks if the blob is a directory
    ///</summary>
    ///<returns>none</returns>
    bool IsDirectory(const char * path) override;
    ///<summary>
    /// Helper function - Checks if the "directory" blob is empty
    ///</summary>
    D_RETURN_CODE IsDirectoryEmpty(std::string path) override;
    ///<summary>
    /// Deletes a File
    ///</summary>
    ///<returns>none</returns>
    void DeleteFile(std::string pathToDelete) override;
    ///<summary>
    /// Gets the properties of a path
    ///</summary>
    ///<returns>BfsFileProperty object which contains the property details of the file</returns>
    BfsFileProperty GetProperties(std::string pathName) override;
    ///<summary>
    /// Determines whether or not a path (file or directory) exists or not
    ///</summary>
    ///<returns>none</returns>
    int Exists(std::string pathName) override;
    ///<summary>
    /// Determines whether or not a path (file or directory) exists or not
    ///</summary>
    ///<returns>none</returns>
    bool Copy(std::string sourcePath, std::string destinationPath) override;
    ///<summary>
    /// Renames a file
    ///</summary>
    ///<returns>none</returns>
    std::vector<std::string> Rename(std::string sourcePath, std::string destinationPath) override;
    ///<summary>
    /// Lists
    ///</summary>
    ///<returns>none</returns>
    list_hierarchical_response List(std::string delimiter, std::string continuation, std::string prefix) const override;
    ///<summary>
    /// LIsts all directories within a list container
    /// Greedily list all blobs using the input params.
    ///</summary>
    std::vector<std::pair<std::vector<list_hierarchical_item>, bool>> ListAllItemsHierarchical(const std::string& delimiter, const std::string& prefix) override;
    ///<summary>
    /// Updates the UNIX-style file mode on a path.
    ///</summary>
    int ChangeMode(const char* path, mode_t mode) override;

private:
    ///<summary>
    /// Helper function - Renames single file
    ///</summary>
    int rename_single_file(std::string src, std::string dst, std::vector<std::string> & files_to_remove_cache);
    ///<summary>
    /// Helper function - Renames directory
    ///</summary>
    int rename_directory(std::string src, std::string dst, std::vector<std::string> & files_to_remove_cache);
    ///<summary>
    /// Helper function - Ensures directory path exists in the cache
    /// TODO: refactoring, rename variables and add comments to make sense to parsing
    ///</summary>
    int ensure_directory_path_exists_cache(const std::string & file_path);
    ///<summary>
    /// Helper function - Authenticates with an account key
    ///</summary>
    std::shared_ptr<sync_blob_client> authenticate_accountkey();
    ///<summary>
    /// Helper function - Authenticates with an account sas
    ///</summary>
    std::shared_ptr<sync_blob_client> authenticate_sas();
    ///<summary>
    /// Helper function - Authenticates with msi
    ///</summary>
    std::shared_ptr<sync_blob_client> authenticate_msi();
    ///<summary>
    /// Helper function - Checks metadata hdi_isfolder aka if the blob marker is a folder
    ///</summary>
    bool is_folder(const std::vector<std::pair<std::string,std::string>> & metadata);
    ///<summary>
    /// Blob Client to make blob storage calls
    ///</summary>
    std::shared_ptr<sync_blob_client> m_blob_client;
};
#endif //BLOCKBLOBBFSCLIENTBASE_H