#pragma once
#include <cpprest/filestream.h>
#include <cpprest/http_client.h>

using namespace utility;              // 文字列変換などの一般的なユーティリティ
using namespace web;                  // URIのような共通の機能
using namespace web::http;            // 共通のHTTP機能
using namespace web::http::client;    // HTTP クライアントの機能
using namespace concurrency::streams; // 非同期ストリーム

template <class T>
pplx::task<T> Get(const std::wstring& url)
{
	return pplx::create_task([=]
		{
			// クライアントの設定
			http_client client(url);

			// リクエスト送信
			return client.request(methods::GET);
		})
		.then([](http_response response)
			{
				if (response.status_code() == status_codes::OK)
				{
					return response.extract_json();
				}
				else
				{
					throw std::runtime_error("Received non-OK HTTP status code");
				}
			}
		);
}

pplx::task<int> Post(const std::wstring& url, int score)
{
	return pplx::create_task([=]
		{
			// 送信データの作成
			json::value postData;
			postData[L"score"] = score;

			// クライアントの設定
			http_client client(url);

			// リクエスト送信
			return client.request(methods::POST, L"", postData.serialize(), L"application/json");
		})
		.then([](http_response response)
			{
				// ステータスコード判定
				if (response.status_code() == status_codes::Created)
				{
					return response.extract_json();
				}
				else
				{
					throw std::runtime_error("Received non-OK HTTP status code");
				}
			})
		.then([](json::value json) { return json[L"status_code"].as_integer(); });
}
