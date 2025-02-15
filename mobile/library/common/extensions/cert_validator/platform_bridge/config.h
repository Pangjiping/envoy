#include "envoy/registry/registry.h"

#include "source/extensions/transport_sockets/tls/cert_validator/factory.h"

#include "library/common/extensions/cert_validator/platform_bridge/platform_bridge_cert_validator.h"

namespace Envoy {
namespace Extensions {
namespace TransportSockets {
namespace Tls {

class PlatformBridgeCertValidatorFactory : public CertValidatorFactory {
public:
  CertValidatorPtr createCertValidator(const Envoy::Ssl::CertificateValidationContextConfig* config,
                                       SslStats& stats, TimeSource& time_source) override;

  std::string name() const override {
    return "envoy_mobile.cert_validator.platform_bridge_cert_validator";
  }

private:
  const envoy_cert_validator* platform_validator_ = nullptr;
};

DECLARE_FACTORY(PlatformBridgeCertValidatorFactory);

} // namespace Tls
} // namespace TransportSockets
} // namespace Extensions
} // namespace Envoy
