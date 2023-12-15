 /*
 WiFiClientSecure encrypts connections to remote servers (eg github, smtp)
 To optionally validate identity of remote server (prevent man-in-middle threats), 
 its public certificate needs to be checked by the app.
 Use openssl tool to obtain public certificate of remote server, eg:
   openssl s_client -showcerts -verify 5 -connect raw.githubusercontent.com:443
   openssl s_client -showcerts -verify 5 -connect smtp.gmail.com:465
 Copy and paste last listed certificate (usually root CA certificate) into relevant constant below.
 To disable certificate checking (WiFiClientSecure) leave relevant constant empty, and / or
 on web page under Access Settings / Authentication settings set Use Secure to off

 FTP connection is plaintext as FTPS not implemented.


 To set app as HTTPS server, a server private key and public certificate are required
 Create keys and certificates using openssl tool

 Define app to have static IP address, and use as variable substitution for openssl:
   APP_IP="192.168.1.133"
 Create app server private key and public certificate:
   openssl req -nodes -x509 -sha256 -newkey rsa:4096 -subj "/CN='${APP_IP}'" -extensions v3_ca -keyout prvtkey.pem -out cacert.pem -days 800
 
 Paste content of prvtkey.pem and cacert.pem files into prvtkey_pem and cacert_pem constants below.
 View server cert content:
   openssl x509 -in cacert.pem -noout -text

 Use of HTTPS is controlled on web page by option 'Use HTTPS' under Access Settings / Authentication settings 
 If the private key or public certificate constants are empty, the Use HTTPS setting is ignored.

 s60sc 2023
 */

#include "appGlobals.h"

// GitHub public certificate valid till April 2031
const char* git_rootCACertificate = R"~(
-----BEGIN CERTIFICATE-----
MIIEvjCCA6agAwIBAgIQBtjZBNVYQ0b2ii+nVCJ+xDANBgkqhkiG9w0BAQsFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0yMTA0MTQwMDAwMDBaFw0zMTA0MTMyMzU5NTlaME8xCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxKTAnBgNVBAMTIERpZ2lDZXJ0IFRMUyBS
U0EgU0hBMjU2IDIwMjAgQ0ExMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKC
AQEAwUuzZUdwvN1PWNvsnO3DZuUfMRNUrUpmRh8sCuxkB+Uu3Ny5CiDt3+PE0J6a
qXodgojlEVbbHp9YwlHnLDQNLtKS4VbL8Xlfs7uHyiUDe5pSQWYQYE9XE0nw6Ddn
g9/n00tnTCJRpt8OmRDtV1F0JuJ9x8piLhMbfyOIJVNvwTRYAIuE//i+p1hJInuW
raKImxW8oHzf6VGo1bDtN+I2tIJLYrVJmuzHZ9bjPvXj1hJeRPG/cUJ9WIQDgLGB
Afr5yjK7tI4nhyfFK3TUqNaX3sNk+crOU6JWvHgXjkkDKa77SU+kFbnO8lwZV21r
eacroicgE7XQPUDTITAHk+qZ9QIDAQABo4IBgjCCAX4wEgYDVR0TAQH/BAgwBgEB
/wIBADAdBgNVHQ4EFgQUt2ui6qiqhIx56rTaD5iyxZV2ufQwHwYDVR0jBBgwFoAU
A95QNVbRTLtm8KPiGxvDl7I90VUwDgYDVR0PAQH/BAQDAgGGMB0GA1UdJQQWMBQG
CCsGAQUFBwMBBggrBgEFBQcDAjB2BggrBgEFBQcBAQRqMGgwJAYIKwYBBQUHMAGG
GGh0dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNvbTBABggrBgEFBQcwAoY0aHR0cDovL2Nh
Y2VydHMuZGlnaWNlcnQuY29tL0RpZ2lDZXJ0R2xvYmFsUm9vdENBLmNydDBCBgNV
HR8EOzA5MDegNaAzhjFodHRwOi8vY3JsMy5kaWdpY2VydC5jb20vRGlnaUNlcnRH
bG9iYWxSb290Q0EuY3JsMD0GA1UdIAQ2MDQwCwYJYIZIAYb9bAIBMAcGBWeBDAEB
MAgGBmeBDAECATAIBgZngQwBAgIwCAYGZ4EMAQIDMA0GCSqGSIb3DQEBCwUAA4IB
AQCAMs5eC91uWg0Kr+HWhMvAjvqFcO3aXbMM9yt1QP6FCvrzMXi3cEsaiVi6gL3z
ax3pfs8LulicWdSQ0/1s/dCYbbdxglvPbQtaCdB73sRD2Cqk3p5BJl+7j5nL3a7h
qG+fh/50tx8bIKuxT8b1Z11dmzzp/2n3YWzW2fP9NsarA4h20ksudYbj/NhVfSbC
EXffPgK2fPOre3qGNm+499iTcc+G33Mw+nur7SpZyEKEOxEXGlLzyQ4UfaJbcme6
ce1XR2bFuAJKZTRei9AqPCCcUZlM51Ke92sRKw2Sfh3oius2FkOH6ipjv3U/697E
A7sKPPcw7+uvTPyLNhBzPvOk
-----END CERTIFICATE-----
)~";


// Paste in app server private key
const char* prvtkey_pem = R"~(
-----BEGIN PRIVATE KEY-----
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
PtFZFFUE2Hp/A5xgVaY3L4Jx/zjzog==
-----END PRIVATE KEY-----
)~";

// Paste in self signed app server public certificate
const char* cacert_pem = R"~(
-----BEGIN CERTIFICATE-----
MIIFFTCCAv2gAwIBAgIUMhtEcF7uxs95iP0dWqnHWyZOa3AwDQYJKoZIhvcNAQEL
BQAwGjEYMBYGA1UEAwwPJzE5Mi4xNjguMS4xMzUnMB4XDTIzMTEyOTE4MTg0NFoX
DTI2MDIwNjE4MTg0NFowGjEYMBYGA1UEAwwPJzE5Mi4xNjguMS4xMzUnMIICIjAN
BgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEA7W0kXWBiyOEaNJE0vQimYR+TpBfZ
462iEGqxI0FRDd2AHMBzz7BG0BRBaLxKN9/IAz93OmxwcsG8XTv6Cn+08ihw638l
sH2ovOqBlGWDdOm8l8ladBP7i1Mt5DfH/rkZigF9n5w6IpYofw3YzisDPLBN+xgr
i+UrYBnLWN+gGr56BpTue+X7ZUqSo9PkVv3C5jxkgP1M22BwouZxL890GcuXlrpd
2Hn9Bf3ME9OXil5I2ypgD2FX/estGr14vK6726dcdQnef5Fx+H2bYzqolLDOKcMS
yWsWhoBKYp0B+sARtnqSgqDHdUfV5b8iMFnOimw6DjrojpsAl4OhkhGGRsz7Axf6
DXBxjLyPFk+xx+iM5vJjUF3yJdxYEHPMcYFAuL+rFQc2G3TeZ4jdHEOsPQE8DQb9
2Azi+FnxqqBBkeqYcq1jXVWuPNeHZuleizueSj/IAHZxorzRF4gnY4Cqz80BD+lb
+ecBSeD5iun1Dv79m9W0l47LlbC9pPNu88aZYyHVV7P5SaL3YFqFu8yX8/nS7TrT
/OUTJ7KVFCUGHpLQO/APKnlFOSCaq2X4cSBBvE7J1u8mtlFBk7CIezH+/8UUk9jX
PbcjOo9LG2rUv4wlFgq3TvuN7gZqck0Nu2SoAzrtrPSNs/Ynsb3w31j7/9fNQvBN
+MniDLZEQ+gpmfECAwEAAaNTMFEwHQYDVR0OBBYEFNPhBiWpysVJuCiDzXEFzW4d
Sp5SMB8GA1UdIwQYMBaAFNPhBiWpysVJuCiDzXEFzW4dSp5SMA8GA1UdEwEB/wQF
MAMBAf8wDQYJKoZIhvcNAQELBQADggIBAEW04/FeWRDDblTUi/lBoy4QJNVB5mAW
7zW1F7CvFCqLgWXzVGnmq4sdfH51YDPc47dozmSxBoaFQXX3nczLZHpAuLrE5jWo
Zc3atnWzgu0T5nB/bsVIb7VHbpUNlzJBuh8GHFhi82Uxqyv+5cAREbtnYtVrf5kZ
qRbewnxiQ7C2UT3Fofzd8xNz3o0Q2aTs5Ml++Wt/JF+lPC3Psdtp38p33bHUhqEJ
0ddxUfhKIq5CMmHw62wZFTJO3e6xCk5R3O1YMN9rq7zPtjgaoj19fy5ciSqY9UIC
ZTmLsLJqroRDkcH5a595OXCFMq3+xtC8642qyapl4MM2f3VzgKdlag33qPzxqQQm
gyc62FODRODCm7vAG7jpj0/kq6SH68AdgvWGlHv+lYfnkPDP+6ThjKIN4RyY4qmx
sutvn/I7lLg4xg1h8jvL1zhRaYvfIfbT8CLEDhgVq+PzvNctGP6+5KTmZOgvUspo
sZZWQwdV3epQVA9kmoghyEfLf03LhQw43dpbH+12ShTLgYBUn/CSSuHpjFWylalH
RK4XUkDMYRkfYEA6ZS5/4azkQaVWOqtNlJpCYowXcAG/YA7OEycsHhEtPRZJ6/9/
89T0MEsrJBrJv4rgp+kvS0hHoU3gGKhiTFyIeT8YFySVPZ5EnbJxocibLN4HHt5L
wiiNAzvzYe9M
-----END CERTIFICATE-----
)~";


// Your FTPS Server's root public certificate (not implemented)
const char* ftps_rootCACertificate = R"~(
)~";


// Your SMTP Server's public certificate (eg smtp.gmail.com valid till Jan 2028)
const char* smtp_rootCACertificate = R"~(
-----BEGIN CERTIFICATE-----
MIIFYjCCBEqgAwIBAgIQd70NbNs2+RrqIQ/E8FjTDTANBgkqhkiG9w0BAQsFADBX
MQswCQYDVQQGEwJCRTEZMBcGA1UEChMQR2xvYmFsU2lnbiBudi1zYTEQMA4GA1UE
CxMHUm9vdCBDQTEbMBkGA1UEAxMSR2xvYmFsU2lnbiBSb290IENBMB4XDTIwMDYx
OTAwMDA0MloXDTI4MDEyODAwMDA0MlowRzELMAkGA1UEBhMCVVMxIjAgBgNVBAoT
GUdvb2dsZSBUcnVzdCBTZXJ2aWNlcyBMTEMxFDASBgNVBAMTC0dUUyBSb290IFIx
MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAthECix7joXebO9y/lD63
ladAPKH9gvl9MgaCcfb2jH/76Nu8ai6Xl6OMS/kr9rH5zoQdsfnFl97vufKj6bwS
iV6nqlKr+CMny6SxnGPb15l+8Ape62im9MZaRw1NEDPjTrETo8gYbEvs/AmQ351k
KSUjB6G00j0uYODP0gmHu81I8E3CwnqIiru6z1kZ1q+PsAewnjHxgsHA3y6mbWwZ
DrXYfiYaRQM9sHmklCitD38m5agI/pboPGiUU+6DOogrFZYJsuB6jC511pzrp1Zk
j5ZPaK49l8KEj8C8QMALXL32h7M1bKwYUH+E4EzNktMg6TO8UpmvMrUpsyUqtEj5
cuHKZPfmghCN6J3Cioj6OGaK/GP5Afl4/Xtcd/p2h/rs37EOeZVXtL0m79YB0esW
CruOC7XFxYpVq9Os6pFLKcwZpDIlTirxZUTQAs6qzkm06p98g7BAe+dDq6dso499
iYH6TKX/1Y7DzkvgtdizjkXPdsDtQCv9Uw+wp9U7DbGKogPeMa3Md+pvez7W35Ei
Eua++tgy/BBjFFFy3l3WFpO9KWgz7zpm7AeKJt8T11dleCfeXkkUAKIAf5qoIbap
sZWwpbkNFhHax2xIPEDgfg1azVY80ZcFuctL7TlLnMQ/0lUTbiSw1nH69MG6zO0b
9f6BQdgAmD06yK56mDcYBZUCAwEAAaOCATgwggE0MA4GA1UdDwEB/wQEAwIBhjAP
BgNVHRMBAf8EBTADAQH/MB0GA1UdDgQWBBTkrysmcRorSCeFL1JmLO/wiRNxPjAf
BgNVHSMEGDAWgBRge2YaRQ2XyolQL30EzTSo//z9SzBgBggrBgEFBQcBAQRUMFIw
JQYIKwYBBQUHMAGGGWh0dHA6Ly9vY3NwLnBraS5nb29nL2dzcjEwKQYIKwYBBQUH
MAKGHWh0dHA6Ly9wa2kuZ29vZy9nc3IxL2dzcjEuY3J0MDIGA1UdHwQrMCkwJ6Al
oCOGIWh0dHA6Ly9jcmwucGtpLmdvb2cvZ3NyMS9nc3IxLmNybDA7BgNVHSAENDAy
MAgGBmeBDAECATAIBgZngQwBAgIwDQYLKwYBBAHWeQIFAwIwDQYLKwYBBAHWeQIF
AwMwDQYJKoZIhvcNAQELBQADggEBADSkHrEoo9C0dhemMXoh6dFSPsjbdBZBiLg9
NR3t5P+T4Vxfq7vqfM/b5A3Ri1fyJm9bvhdGaJQ3b2t6yMAYN/olUazsaL+yyEn9
WprKASOshIArAoyZl+tJaox118fessmXn1hIVw41oeQa1v1vg4Fv74zPl6/AhSrw
9U5pCZEt4Wi4wStz6dTZ/CLANx8LZh1J7QJVj2fhMtfTJr9w4z30Z209fOU0iOMy
+qduBmpvvYuR7hZL6Dupszfnw0Skfths18dG9ZKb59UhvmaSGZRVbNQpsg3BZlvi
d0lIKO2d1xozclOzgjXPYovJJIultzkMu34qQb9Sz/yilrbCgj8=
-----END CERTIFICATE-----
)~";


// Your MQTT Server's public certificate 
const char* mqtt_rootCACertificate = R"~(
)~";

// Telegram server certificate for api.telegram.org, valid till May 2031
const char* telegram_rootCACertificate = R"~(
-----BEGIN CERTIFICATE-----
MIIEfTCCA2WgAwIBAgIDG+cVMA0GCSqGSIb3DQEBCwUAMGMxCzAJBgNVBAYTAlVT
MSEwHwYDVQQKExhUaGUgR28gRGFkZHkgR3JvdXAsIEluYy4xMTAvBgNVBAsTKEdv
IERhZGR5IENsYXNzIDIgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTQwMTAx
MDcwMDAwWhcNMzEwNTMwMDcwMDAwWjCBgzELMAkGA1UEBhMCVVMxEDAOBgNVBAgT
B0FyaXpvbmExEzARBgNVBAcTClNjb3R0c2RhbGUxGjAYBgNVBAoTEUdvRGFkZHku
Y29tLCBJbmMuMTEwLwYDVQQDEyhHbyBEYWRkeSBSb290IENlcnRpZmljYXRlIEF1
dGhvcml0eSAtIEcyMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAv3Fi
CPH6WTT3G8kYo/eASVjpIoMTpsUgQwE7hPHmhUmfJ+r2hBtOoLTbcJjHMgGxBT4H
Tu70+k8vWTAi56sZVmvigAf88xZ1gDlRe+X5NbZ0TqmNghPktj+pA4P6or6KFWp/
3gvDthkUBcrqw6gElDtGfDIN8wBmIsiNaW02jBEYt9OyHGC0OPoCjM7T3UYH3go+
6118yHz7sCtTpJJiaVElBWEaRIGMLKlDliPfrDqBmg4pxRyp6V0etp6eMAo5zvGI
gPtLXcwy7IViQyU0AlYnAZG0O3AqP26x6JyIAX2f1PnbU21gnb8s51iruF9G/M7E
GwM8CetJMVxpRrPgRwIDAQABo4IBFzCCARMwDwYDVR0TAQH/BAUwAwEB/zAOBgNV
HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFDqahQcQZyi27/a9BUFuIMGU2g/eMB8GA1Ud
IwQYMBaAFNLEsNKR1EwRcbNhyz2h/t2oatTjMDQGCCsGAQUFBwEBBCgwJjAkBggr
BgEFBQcwAYYYaHR0cDovL29jc3AuZ29kYWRkeS5jb20vMDIGA1UdHwQrMCkwJ6Al
oCOGIWh0dHA6Ly9jcmwuZ29kYWRkeS5jb20vZ2Ryb290LmNybDBGBgNVHSAEPzA9
MDsGBFUdIAAwMzAxBggrBgEFBQcCARYlaHR0cHM6Ly9jZXJ0cy5nb2RhZGR5LmNv
bS9yZXBvc2l0b3J5LzANBgkqhkiG9w0BAQsFAAOCAQEAWQtTvZKGEacke+1bMc8d
H2xwxbhuvk679r6XUOEwf7ooXGKUwuN+M/f7QnaF25UcjCJYdQkMiGVnOQoWCcWg
OJekxSOTP7QYpgEGRJHjp2kntFolfzq3Ms3dhP8qOCkzpN1nsoX+oYggHFCJyNwq
9kIDN0zmiN/VryTyscPfzLXs4Jlet0lUIDyUGAzHHFIYSaRt4bNYC8nY7NmuHDKO
KHAN4v6mF56ED71XcLNa6R+ghlO773z/aQvgSMO3kwvIClTErF0UZzdsyqUvMQg3
qm5vjLyb4lddJIGvl5echK1srDdMZvNhkREg5L4wn3qkKQmw4TRfZHcYQFHfjDCm
rw==
-----END CERTIFICATE-----
)~";


// Your HTTPS File Server public certificate 
const char* hfs_rootCACertificate = R"~(
-----BEGIN CERTIFICATE-----
MIIDKzCCAhOgAwIBAgIQJOOyowYpLEiLnhpjD0HR5DANBgkqhkiG9w0BAQsFADAg
MR4wHAYDVQQDExVSZWJleCBUaW55IFdlYiBTZXJ2ZXIwHhcNMjMxMTI1MTQ1NzU4
WhcNMzMxMTI1MTQ1NzU4WjAgMR4wHAYDVQQDExVSZWJleCBUaW55IFdlYiBTZXJ2
ZXIwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDGgUPfvM+SI1OiOozW
MQUtj/E9461SRFIYfce7b0KANrc/0S4qD11tTQvKvoY7HY5gvc0jgWZBGqQ6/KO7
CIrLnXtsldeUXokcd8cvr8Ko704iOboHLLhewZ4egeBgu6+/1dw6REeExHjNIjiO
1InShPIV8yxX1oUo+EztIo5qecVvrousyIL9KBmAAi7Pdw0/yKQaoXzL9Ehkpv7g
Pbabt6k7W+CofXRhaoGlf5ERvb5T/921PXXdCq7mnB9OjGu0almqYIWh1jRjnBQH
e2avg+LD/+1dakIXXzByhbEpECxtQHZ17iB3DvW0ExiMH+0A8bqQIMp3sOgEzf2J
42XpAgMBAAGjYTBfMA4GA1UdDwEB/wQEAwIHgDAdBgNVHQ4EFgQUUmrFj3+h5tsV
ASwGkjFZ9W5FDf8wEwYDVR0lBAwwCgYIKwYBBQUHAwEwGQYDVR0RBBIwEIIJbG9j
YWxob3N0ggNtcG8wDQYJKoZIhvcNAQELBQADggEBAJ+rf1UUwaZAhsHrL2KX0WPm
E9lCcnBFeQHUILSfM7r7fbEuIXa68mZDeMIV9xs4ex45wN4AAZW1l79Okia9kin8
JkqkhZ/rCvqWsbNt3ryOvWCB2a2JEWW6yRA6EgK+STo3T/Z8Sau0ys8woc7y486l
5BhGu7rlXcbXl8hcEORD/ILxxdae7hHi7sXIReyS2kGiYJUwj+1+6mm26TXuRyCV
jqlsBxH8gnwIlupODKZ/7jU/HhiYaKEbrnNxiOiPeWAw/KJJH5lUxt0piOYIXhj4
DuDay+U7jeJKpND7EYheZY/U6c1wqwXt1DHuFnCCzK8jdOGT9aUSqZUeWfNn9cc=
-----END CERTIFICATE-----
)~";