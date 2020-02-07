#include <string>
#include <dirent.h>
#include <vector>
#include <syslog.h>
#include "Constants.h"
#include "file_lock_map.h"
#include "storage_errno.h"

//TODO: to remove once we unify list responses
#include "list_blobs_request_base.h"

#ifndef STORAGEBFSCLIENTBASE_H
#define STORAGEBFSCLIENTBASE_H

// Global struct storing the Storage connection information and the tmpPath.
struct str_options
{
    std::string accountName;
    AUTH_TYPE authType;
    std::string blobEndpoint;
    std::string accountKey;
    std::string sasToken;
    std::string clientId;
    std::string objectId;
    std::string resourceId;
    std::string msiEndpoint;
    std::string containerName;
    std::string tmpPath;
    int fileCacheTimeoutInSeconds;
    bool useHttps;
    bool useAttrCache;
    //this is set by the --allow-other flag,
    // 0770 if not set, 0777 if the flag is set
    int defaultPermission;
};

struct BfsFileProperty
{
    BfsFileProperty() : m_valid(false) {}
    BfsFileProperty(std::string cache_control,
                std::string content_disposition,
                std::string content_encoding,
                std::string content_language,
                std::string content_md5,
                std::string content_type,
                std::string etag,
                std::string copy_status,
                std::vector<std::pair<std::string, std::string>> metadata,
                time_t last_modified,
                std::string modestring,
                unsigned long long size) :
                m_cache_control(cache_control),
                m_content_disposition(content_disposition),
                m_content_encoding(content_encoding),
                m_content_language(content_language),
                m_content_md5(content_md5),
                m_content_type(content_type),
                m_etag(etag),
                m_copy_status(copy_status),
                m_metadata(metadata),
                m_last_modified(last_modified),
                m_size(size),
                m_valid(true) {
        if (!modestring.empty()) {
            m_file_mode = 0000; // Supply no file mode to begin with unless the mode string is empty
            for (char & c : modestring) {
                // Start by pushing back the mode_t.
                m_file_mode = m_file_mode << 1; // NOLINT(hicpp-signed-bitwise) (mode_t is signed, apparently. Suppress the inspection.)
                // Then flip the new bit based on whether the mode is enabled or not.
                // This works because we can expect a consistent 9 character modestring.
                m_file_mode |= (c != '-');
            }
        }
    }

    std::string m_cache_control;
    std::string m_content_disposition;
    std::string m_content_encoding;
    std::string m_content_language;
    std::string m_content_md5;
    std::string m_content_type;
    std::string m_etag;
    std::string m_copy_status;
    std::vector<std::pair<std::string, std::string>> m_metadata;
    time_t m_last_modified;
    mode_t m_file_mode;
    unsigned long long m_size;
    bool m_valid;

    bool isValid()
    {
        return m_valid;
    }

    unsigned long long size()
    {
        return m_size;
    }

    time_t last_modified()
    {
        return m_last_modified;
    }


};

struct list_hierarchical_item {
    list_hierarchical_item(list_blobs_hierarchical_item);
    std::string name;
    std::string snapshot;
    std::string last_modified;
    std::string etag;
    unsigned long long content_length;
    std::string content_encoding;
    std::string content_type;
    std::string content_md5;
    std::string content_language;
    std::string cache_control;
    std::string copy_status;
    std::vector<std::pair<std::string, std::string>> metadata;
    bool is_directory;
};

struct list_hierarchical_response {
    list_hierarchical_response() : m_valid(false) {}
    list_hierarchical_response(list_blobs_hierarchical_response response);
    std::string m_ms_request_id;
    std::vector<list_hierarchical_item> m_items;
    std::string m_next_marker;
    bool m_valid;
};

class StorageBfsClientBase
{
public:
    StorageBfsClientBase(str_options str_options) : configurations(str_options)
    {}
    ///<summary>
    /// Authenticates the storage account and container
    ///</summary>
    ///<returns>bool: if we authenticate to the storage account and container successfully</returns>
    virtual bool AuthenticateStorage() = 0;
    ///<summary>
    /// Uploads contents of a file to a storage object(e.g. blob, file) to the Storage service
    ///</summary>
    ///TODO: params
    ///<returns>none</returns>
    virtual void UploadFromFile(const std::string localPath) = 0;
    ///<summary>
    /// Uploads contents of a stream to a storage object(e.g. blob, file) to the Storage service
    ///</summary>
    ///<returns>none</returns>
    virtual void UploadFromStream(std::istream & sourceStream, const std::string blobName) = 0;
    ///<summary>
    /// Downloads contents of a storage object(e.g. blob, file) to a local file
    ///</summary>
    ///<returns>none</returns>
    virtual void DownloadToFile(const std::string blobName, const std::string filePath) = 0;
    ///<summary>
    /// Creates a Directory
    ///</summary>
    ///<returns>none</returns>
    virtual bool CreateDirectory(const std::string directoryPath) = 0;
    ///<summary>
    /// Deletes a Directory
    ///</summary>
    ///<returns>none</returns>
    virtual bool DeleteDirectory(const std::string directoryPath) = 0;
    ///<summary>
    /// Checks if the blob is a directory
    ///</summary>
    ///<returns>none</returns>
    virtual bool IsDirectory(const char * path) = 0;
    ///<summary>
    /// Helper function - Checks if the "directory" blob is empty
    ///</summary>
    virtual D_RETURN_CODE IsDirectoryEmpty(std::string path) = 0;
    ///<summary>
    /// Deletes a File
    ///</summary>
    ///<returns>none</returns>
    virtual void DeleteFile(const std::string pathToDelete) = 0;
    ///<summary>
    /// Determines whether or not a path (file or directory) exists or not
    ///</summary>
    ///<returns>none</returns>
    virtual int Exists(const std::string pathName) = 0;
    ///<summary>
    /// Gets the properties of a path
    ///</summary>
    ///<returns>BfsFileProperty object which contains the property details of the file</returns>
    virtual BfsFileProperty GetProperties(const std::string pathName) = 0;
    ///<summary>
    /// Determines whether or not a path (file or directory) exists or not
    ///</summary>
    ///<returns>none</returns>
    virtual bool Copy(const std::string sourcePath, const std::string destinationPath) = 0;
    ///<summary>
    /// Renames a file
    ///</summary>
    ///<returns>none</returns>
    virtual std::vector<std::string> Rename(const std::string sourcePath,const  std::string destinationPath) = 0;
    ///<summary>
    /// Lists
    ///</summary>
    ///<returns>none</returns>
    virtual list_hierarchical_response List(const std::string delimiter, std::string continuation, const std::string prefix) const = 0;
    ///<summary>
    /// LIsts all directories within a list container
    /// Greedily list all blobs using the input params.
    ///</summary>
    virtual std::vector<std::pair<std::vector<list_hierarchical_item>, bool>> ListAllItemsHierarchical(const std::string& delimiter, const std::string& prefix) = 0;
    ///<summary>
    /// Updates the UNIX-style file mode on a path.
    ///</summary>
    virtual int ChangeMode(const char* path, mode_t mode) = 0;

protected:
    str_options configurations;

    ///<summary>
    /// Helper function - To map errno
    ///</summary>
    int map_errno(int error);
    ///<summary>
    /// Helper function - To append root foolder to ache to cache folder
    ///</summary>
    std::string prepend_mnt_path_string(const std::string& path);
};

#endif