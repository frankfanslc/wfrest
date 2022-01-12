#include "workflow/WFFacilities.h"
#include <gtest/gtest.h>
#include "wfrest/HttpServer.h"
#include "wfrest/ErrorCode.h"

using namespace wfrest;
using namespace protocol;

WFHttpTask *create_http_task(const std::string &path)
{
    return WFTaskFactory::create_http_task("http://127.0.0.1:8888/" + path, 4, 2, nullptr);
}

TEST(HttpServer, proxy)
{
    HttpServer svr;
    WFFacilities::WaitGroup wait_group(1);

    svr.GET("/test", [](const HttpReq *req, HttpResp *resp)
    {
        resp->String("test");
    });

    svr.GET("/proxy", [](const HttpReq *req, HttpResp *resp)
    {
        resp->Http("http://127.0.0.1:8888/test");
    });

    EXPECT_TRUE(svr.start("127.0.0.1", 8888) == 0) << "http server start failed";

    WFHttpTask *client_task = create_http_task("proxy");
    client_task->set_callback([&wait_group](WFHttpTask *task)
    {
        HttpRequest *req = task->get_req();
        HttpResponse *resp = task->get_resp();

        const void *body;
        size_t body_len;

        resp->get_parsed_body(&body, &body_len);
        fprintf(stderr, "%s\n", static_cast<const char *>(body));
        EXPECT_TRUE(strcmp("test", static_cast<const char *>(body)) == 0);
        wait_group.done();
    });

    client_task->start();
    wait_group.wait();
    svr.stop();
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}