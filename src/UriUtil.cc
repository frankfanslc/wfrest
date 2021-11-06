//
// Created by Chanchan on 11/4/21.
//

#include "UriUtil.h"
#include "workflow/StringUtil.h"

using namespace wfrest;

std::unordered_map<std::string, std::string> UriUtil::split_query(const std::string &query)
{
    std::unordered_map<std::string, std::string> res;

    if (query.empty())
        return res;

    std::vector<std::string> arr = StringUtil::split(query, '&');

    if (arr.empty())
        return res;

    for (const auto& ele : arr)
    {
        if (ele.empty())
            continue;

        std::vector<std::string> kv = StringUtil::split(ele, '=');
        size_t kv_size = kv.size();
        std::string& key = kv[0];

        if (key.empty() || res.count(key) > 0)
            continue;

        if (kv_size == 1)
        {
            res.emplace(std::move(key), "");
            continue;
        }

        std::string& val = kv[1];

        if (val.empty())
            res.emplace(std::move(key), "");
        else
            res.emplace(std::move(key), std::move(val));
    }

    return res;
}