#pragma once
#include <cpprest/filestream.h>
#include <cpprest/http_client.h>

using namespace utility;              // ������ϊ��Ȃǂ̈�ʓI�ȃ��[�e�B���e�B
using namespace web;                  // URI�̂悤�ȋ��ʂ̋@�\
using namespace web::http;            // ���ʂ�HTTP�@�\
using namespace web::http::client;    // HTTP �N���C�A���g�̋@�\
using namespace concurrency::streams; // �񓯊��X�g���[��

template <class T>
pplx::task<T> Get(const std::wstring& url)
{
	return pplx::create_task([=]
		{
			// �N���C�A���g�̐ݒ�
			http_client client(url);

			// ���N�G�X�g���M
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
			// ���M�f�[�^�̍쐬
			json::value postData;
			postData[L"score"] = score;

			// �N���C�A���g�̐ݒ�
			http_client client(url);

			// ���N�G�X�g���M
			return client.request(methods::POST, L"", postData.serialize(), L"application/json");
		})
		.then([](http_response response)
			{
				// �X�e�[�^�X�R�[�h����
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
