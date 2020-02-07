//
// Created by adreed on 2/7/2020.
//

#include <get_dfs_properties_request.h>
#include "utility.h"

namespace microsoft_azure {
    namespace storage_adls {
        void get_dfs_properties_request::build_request(const storage_account &account, http_base &http) const {
            http.set_method(http_base::http_method::head);

            storage_url url = account.get_url(storage_account::service::adls);
            url.append_path(m_filesystem).append_path(m_path);

            http.set_url(url.to_string());

            storage_headers headers;
            http.add_header(constants::header_user_agent, constants::header_value_user_agent);
            add_ms_header(http, headers, constants::header_ms_date, get_ms_date(date_format::rfc_1123));
            add_ms_header(http, headers, constants::header_ms_version, constants::header_value_storage_version);

            account.credential()->sign_request(*this, http, url, headers);
        }
    }
}