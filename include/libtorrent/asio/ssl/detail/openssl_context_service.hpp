//
// openssl_context_service.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2005 Voipster / Indrek dot Juhani at voipster dot com
// Copyright (c) 2005 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_SSL_DETAIL_OPENSSL_CONTEXT_SERVICE_HPP
#define ASIO_SSL_DETAIL_OPENSSL_CONTEXT_SERVICE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "asio/detail/push_options.hpp"

#include "asio/detail/push_options.hpp"
#include <cstring>
#include <string>
#include <boost/function.hpp>
#include "asio/detail/pop_options.hpp"

#include "asio/error.hpp"
#include "asio/io_service.hpp"
#include "asio/ssl/context_base.hpp"
#include "asio/ssl/detail/openssl_init.hpp"
#include "asio/ssl/detail/openssl_types.hpp"

namespace asio {
namespace ssl {
namespace detail {

class openssl_context_service
  : public asio::io_service::service
{
public:
  // The native type of the context.
  typedef ::SSL_CTX* impl_type;

  // The type for the password callback function object.
  typedef boost::function<std::string(std::size_t,
      context_base::password_purpose)> password_callback_type;

  // Constructor.
  openssl_context_service(asio::io_service& io_service)
    : asio::io_service::service(io_service)
  {
  }

  // Destroy all user-defined handler objects owned by the service.
  void shutdown_service()
  {
  }

  // Return a null context implementation.
  static impl_type null()
  {
    return 0;
  }

  // Create a new context implementation.
  void create(impl_type& impl, context_base::method m)
  {
    ::SSL_METHOD* ssl_method = 0;
    switch (m)
    {
    case context_base::sslv2:
      ssl_method = ::SSLv2_method();
      break;
    case context_base::sslv2_client:
      ssl_method = ::SSLv2_client_method();
      break;
    case context_base::sslv2_server:
      ssl_method = ::SSLv2_server_method();
      break;
    case context_base::sslv3:
      ssl_method = ::SSLv3_method();
      break;
    case context_base::sslv3_client:
      ssl_method = ::SSLv3_client_method();
      break;
    case context_base::sslv3_server:
      ssl_method = ::SSLv3_server_method();
      break;
    case context_base::tlsv1:
      ssl_method = ::TLSv1_method();
      break;
    case context_base::tlsv1_client:
      ssl_method = ::TLSv1_client_method();
      break;
    case context_base::tlsv1_server:
      ssl_method = ::TLSv1_server_method();
      break;
    case context_base::sslv23:
      ssl_method = ::SSLv23_method();
      break;
    case context_base::sslv23_client:
      ssl_method = ::SSLv23_client_method();
      break;
    case context_base::sslv23_server:
      ssl_method = ::SSLv23_server_method();
      break;
    default:
      break;
    }
    impl = ::SSL_CTX_new(ssl_method);
  }

  // Destroy a context implementation.
  void destroy(impl_type& impl)
  {
    if (impl != null())
    {
      if (impl->default_passwd_callback_userdata)
      {
        password_callback_type* callback =
          static_cast<password_callback_type*>(
              impl->default_passwd_callback_userdata);
        delete callback;
        impl->default_passwd_callback_userdata = 0;
      }

      ::SSL_CTX_free(impl);
      impl = null();
    }
  }

  // Set options on the context.
  template <typename Error_Handler>
  void set_options(impl_type& impl, context_base::options o,
      Error_Handler error_handler)
  {
    ::SSL_CTX_set_options(impl, o);

    asio::error e;
    error_handler(e);
  }

  // Set peer verification mode.
  template <typename Error_Handler>
  void set_verify_mode(impl_type& impl, context_base::verify_mode v,
      Error_Handler error_handler)
  {
    ::SSL_CTX_set_verify(impl, v, 0);

    asio::error e;
    error_handler(e);
  }

  // Load a certification authority file for performing verification.
  template <typename Error_Handler>
  void load_verify_file(impl_type& impl, const std::string& filename,
      Error_Handler error_handler)
  {
    if (::SSL_CTX_load_verify_locations(impl, filename.c_str(), 0) != 1)
    {
      asio::error e(asio::error::invalid_argument);
      error_handler(e);
      return;
    }

    asio::error e;
    error_handler(e);
  }

  // Add a directory containing certification authority files to be used for
  // performing verification.
  template <typename Error_Handler>
  void add_verify_path(impl_type& impl, const std::string& path,
      Error_Handler error_handler)
  {
    if (::SSL_CTX_load_verify_locations(impl, 0, path.c_str()) != 1)
    {
      asio::error e(asio::error::invalid_argument);
      error_handler(e);
      return;
    }

    asio::error e;
    error_handler(e);
  }

  // Use a certificate from a file.
  template <typename Error_Handler>
  void use_certificate_file(impl_type& impl, const std::string& filename,
      context_base::file_format format, Error_Handler error_handler)
  {
    int file_type;
    switch (format)
    {
    case context_base::asn1:
      file_type = SSL_FILETYPE_ASN1;
      break;
    case context_base::pem:
      file_type = SSL_FILETYPE_PEM;
      break;
    default:
      {
        asio::error e(asio::error::invalid_argument);
        error_handler(e);
        return;
      }
    }

    if (::SSL_CTX_use_certificate_file(impl, filename.c_str(), file_type) != 1)
    {
      asio::error e(asio::error::invalid_argument);
      error_handler(e);
      return;
    }

    asio::error e;
    error_handler(e);
  }

  // Use a certificate chain from a file.
  template <typename Error_Handler>
  void use_certificate_chain_file(impl_type& impl, const std::string& filename,
      Error_Handler error_handler)
  {
    if (::SSL_CTX_use_certificate_chain_file(impl, filename.c_str()) != 1)
    {
      asio::error e(asio::error::invalid_argument);
      error_handler(e);
      return;
    }

    asio::error e;
    error_handler(e);
  }

  // Use a private key from a file.
  template <typename Error_Handler>
  void use_private_key_file(impl_type& impl, const std::string& filename,
      context_base::file_format format, Error_Handler error_handler)
  {
    int file_type;
    switch (format)
    {
    case context_base::asn1:
      file_type = SSL_FILETYPE_ASN1;
      break;
    case context_base::pem:
      file_type = SSL_FILETYPE_PEM;
      break;
    default:
      {
        asio::error e(asio::error::invalid_argument);
        error_handler(e);
        return;
      }
    }

    if (::SSL_CTX_use_PrivateKey_file(impl, filename.c_str(), file_type) != 1)
    {
      asio::error e(asio::error::invalid_argument);
      error_handler(e);
      return;
    }

    asio::error e;
    error_handler(e);
  }

  // Use an RSA private key from a file.
  template <typename Error_Handler>
  void use_rsa_private_key_file(impl_type& impl, const std::string& filename,
      context_base::file_format format, Error_Handler error_handler)
  {
    int file_type;
    switch (format)
    {
    case context_base::asn1:
      file_type = SSL_FILETYPE_ASN1;
      break;
    case context_base::pem:
      file_type = SSL_FILETYPE_PEM;
      break;
    default:
      {
        asio::error e(asio::error::invalid_argument);
        error_handler(e);
        return;
      }
    }

    if (::SSL_CTX_use_RSAPrivateKey_file(
          impl, filename.c_str(), file_type) != 1)
    {
      asio::error e(asio::error::invalid_argument);
      error_handler(e);
      return;
    }

    asio::error e;
    error_handler(e);
  }

  // Use the specified file to obtain the temporary Diffie-Hellman parameters.
  template <typename Error_Handler>
  void use_tmp_dh_file(impl_type& impl, const std::string& filename,
      Error_Handler error_handler)
  {
    ::BIO* bio = ::BIO_new_file(filename.c_str(), "r");
    if (!bio)
    {
      asio::error e(asio::error::invalid_argument);
      error_handler(e);
      return;
    }

    ::DH* dh = ::PEM_read_bio_DHparams(bio, 0, 0, 0);
    if (!dh)
    {
      ::BIO_free(bio);
      asio::error e(asio::error::invalid_argument);
      error_handler(e);
      return;
    }

    ::BIO_free(bio);
    int result = ::SSL_CTX_set_tmp_dh(impl, dh);
    if (result != 1)
    {
      ::DH_free(dh);
      asio::error e(asio::error::invalid_argument);
      error_handler(e);
      return;
    }

    asio::error e;
    error_handler(e);
  }

  static int password_callback(char* buf, int size, int purpose, void* data)
  {
    using namespace std; // For strncat and strlen.

    if (data)
    {
      password_callback_type* callback =
        static_cast<password_callback_type*>(data);
      std::string passwd = (*callback)(static_cast<std::size_t>(size),
          purpose ? context_base::for_writing : context_base::for_reading);
      *buf = '\0';
      strncat(buf, passwd.c_str(), size);
      return strlen(buf);
    }

    return 0;
  }

  // Set the password callback.
  template <typename Password_Callback, typename Error_Handler>
  void set_password_callback(impl_type& impl, Password_Callback callback,
      Error_Handler error_handler)
  {
    // Allocate callback function object if not already present.
    if (impl->default_passwd_callback_userdata)
    {
      password_callback_type* callback_function =
        static_cast<password_callback_type*>(
            impl->default_passwd_callback_userdata);
      *callback_function = callback;
    }
    else
    {
      password_callback_type* callback_function =
        new password_callback_type(callback);
      impl->default_passwd_callback_userdata = callback_function;
    }

    // Set the password callback.
    SSL_CTX_set_default_passwd_cb(impl,
        &openssl_context_service::password_callback);

    asio::error e;
    error_handler(e);
  }

private:
  // Ensure openssl is initialised.
  openssl_init<> init_;
};

} // namespace detail
} // namespace ssl
} // namespace asio

#include "asio/detail/pop_options.hpp"

#endif // ASIO_SSL_DETAIL_OPENSSL_CONTEXT_SERVICE_HPP
