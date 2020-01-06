#pragma once

#include "adls_request_base.h"

namespace microsoft_azure { namespace storage_adls {
    class append_data_request final : public adls_request_base
    {
    public:
        append_data_request(std::string filesystem, std::string file, uint64_t offset, uint64_t length) : m_filesystem(std::move(filesystem)), m_file(std::move(file)), m_offset(offset), m_length(length) {}

        void build_request(const storage_account& account, http_base& http) const override;
    private:
        std::string m_filesystem;
        std::string m_file;
        uint64_t m_offset;
        uint64_t m_length;
    };

}}  // azure::storage_adls