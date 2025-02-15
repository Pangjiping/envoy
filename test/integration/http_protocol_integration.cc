#include "test/integration/http_protocol_integration.h"

#include "absl/strings/str_cat.h"

namespace Envoy {
std::vector<HttpProtocolTestParams> HttpProtocolIntegrationTest::getProtocolTestParams(
    const std::vector<Http::CodecType>& downstream_protocols,
    const std::vector<Http::CodecType>& upstream_protocols) {
  std::vector<HttpProtocolTestParams> ret;

  for (auto ip_version : TestEnvironment::getIpVersionsForTest()) {
    for (auto downstream_protocol : downstream_protocols) {
      for (auto upstream_protocol : upstream_protocols) {
#ifndef ENVOY_ENABLE_QUIC
        if (downstream_protocol == Http::CodecType::HTTP3 ||
            upstream_protocol == Http::CodecType::HTTP3) {
          ENVOY_LOG_MISC(warn, "Skipping HTTP/3 as support is compiled out");
          continue;
        }
#endif

        std::vector<Http1Impl> http1_implementations = {Http1Impl::HttpParser};
        if (downstream_protocol == Http::CodecType::HTTP1 ||
            upstream_protocol == Http::CodecType::HTTP1) {
          http1_implementations.push_back(Http1Impl::BalsaParser);
        }

        std::vector<Http2Impl> http2_implementations = {Http2Impl::Nghttp2};
        std::vector<bool> defer_processing_values = {false};
        if (downstream_protocol == Http::CodecType::HTTP2 ||
            upstream_protocol == Http::CodecType::HTTP2) {
          http2_implementations.push_back(Http2Impl::Oghttp2);
          defer_processing_values.push_back(true);
        }

        for (Http1Impl http1_implementation : http1_implementations) {
          for (Http2Impl http2_implementation : http2_implementations) {
            for (bool defer_processing : defer_processing_values) {
              ret.push_back(HttpProtocolTestParams{ip_version, downstream_protocol,
                                                   upstream_protocol, http1_implementation,
                                                   http2_implementation, defer_processing});
            }
          }
        }
      }
    }
  }
  return ret;
}

absl::string_view upstreamToString(Http::CodecType type) {
  switch (type) {
  case Http::CodecType::HTTP1:
    return "HttpUpstream";
  case Http::CodecType::HTTP2:
    return "Http2Upstream";
  case Http::CodecType::HTTP3:
    return "Http3Upstream";
  }
  return "UnknownUpstream";
}

absl::string_view downstreamToString(Http::CodecType type) {
  switch (type) {
  case Http::CodecType::HTTP1:
    return "HttpDownstream_";
  case Http::CodecType::HTTP2:
    return "Http2Downstream_";
  case Http::CodecType::HTTP3:
    return "Http3Downstream_";
  }
  return "UnknownDownstream";
}

absl::string_view http1ImplementationToString(Http1Impl impl) {
  switch (impl) {
  case Http1Impl::HttpParser:
    return "HttpParser";
  case Http1Impl::BalsaParser:
    return "BalsaParser";
  }
  return "UnknownHttp1Impl";
}

absl::string_view http2ImplementationToString(Http2Impl impl) {
  switch (impl) {
  case Http2Impl::Nghttp2:
    return "Nghttp2";
  case Http2Impl::Oghttp2:
    return "Oghttp2";
  }
  return "UnknownHttp2Impl";
}

std::string HttpProtocolIntegrationTest::protocolTestParamsToString(
    const ::testing::TestParamInfo<HttpProtocolTestParams>& params) {
  return absl::StrCat((params.param.version == Network::Address::IpVersion::v4 ? "IPv4_" : "IPv6_"),
                      downstreamToString(params.param.downstream_protocol),
                      upstreamToString(params.param.upstream_protocol),
                      http1ImplementationToString(params.param.http1_implementation),
                      http2ImplementationToString(params.param.http2_implementation),
                      params.param.defer_processing_backedup_streams ? "WithDeferredProcessing"
                                                                     : "NoDeferredProcessing");
}

} // namespace Envoy
