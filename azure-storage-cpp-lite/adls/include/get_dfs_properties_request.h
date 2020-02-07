//
// Created by adreed on 2/7/2020.
//

#include <string>
#include <adls_request_base.h>

#ifndef BLOBFUSE_GET_DFS_PROPERTIES_REQUEST_H
#define BLOBFUSE_GET_DFS_PROPERTIES_REQUEST_H

namespace microsoft_azure {
    namespace storage_adls {
        struct dfs_properties
        {
            std::string cache_control;
            std::string content_disposition;
            std::string content_encoding;
            std::string content_language;
            unsigned long long content_length;
            std::string content_type;
            std::string content_md5;
            std::string etag;
            std::string resource_type;
            std::vector<std::pair<std::string, std::string>> metadata;
            time_t last_modified;
            std::string owner;
            std::string group;
            std::string permissions;
            std::string acl;
        };

        class get_dfs_properties_request final : public adls_request_base
        {
        public:
            get_dfs_properties_request(std::string filesystem, std::string path) :
                m_filesystem(std::move(filesystem)),
                m_path(std::move(path)) {}

            void build_request(const storage_account& account, http_base& http) const override;
        private:
            std::string m_filesystem;
            std::string m_path;
        };
    }
}

#endif //BLOBFUSE_GET_DFS_PROPERTIES_REQUEST_H
