# Cracking PSK

Quick overview of how to crack PSKs.

## Requirements

- [hashcat](https://hashcat.net/hashcat/)
  - `sudo apt update && sudo apt install hashcat`
- hsxtools
  - `sudo apt update && sudo apt install hsxtools`

## Preparation

1. Capture EAPOL-Key frames and download hccapx file
2. Convert hccapx to hashcat format
   - `hcxhash2cap --hccapx=capture.hccapx`
   - Convert cap to hashcat format at
     [hashcat.net/cap2hashcat](https://hashcat.net/cap2hashcat/)

Use the resulting .hc22000 for cracking.

## Cracking

Hashcat network protocol hash types (-m, --hash-type):

```bash
  16100 | TACACS+                                             | Network Protocol
  11400 | SIP digest authentication (MD5)                     | Network Protocol
   5300 | IKE-PSK MD5                                         | Network Protocol
   5400 | IKE-PSK SHA1                                        | Network Protocol
  25100 | SNMPv3 HMAC-MD5-96                                  | Network Protocol
  25000 | SNMPv3 HMAC-MD5-96/HMAC-SHA1-96                     | Network Protocol
  25200 | SNMPv3 HMAC-SHA1-96                                 | Network Protocol
  26700 | SNMPv3 HMAC-SHA224-128                              | Network Protocol
  26800 | SNMPv3 HMAC-SHA256-192                              | Network Protocol
  26900 | SNMPv3 HMAC-SHA384-256                              | Network Protocol
  27300 | SNMPv3 HMAC-SHA512-384                              | Network Protocol
   2500 | WPA-EAPOL-PBKDF2                                    | Network Protocol
   2501 | WPA-EAPOL-PMK                                       | Network Protocol
  22000 | WPA-PBKDF2-PMKID+EAPOL                              | Network Protocol
  22001 | WPA-PMK-PMKID+EAPOL                                 | Network Protocol
  16800 | WPA-PMKID-PBKDF2                                    | Network Protocol
  16801 | WPA-PMKID-PMK                                       | Network Protocol
   7300 | IPMI2 RAKP HMAC-SHA1                                | Network Protocol
  10200 | CRAM-MD5                                            | Network Protocol
  16500 | JWT (JSON Web Token)                                | Network Protocol
  19600 | Kerberos 5, etype 17, TGS-REP                       | Network Protocol
  19800 | Kerberos 5, etype 17, Pre-Auth                      | Network Protocol
  19700 | Kerberos 5, etype 18, TGS-REP                       | Network Protocol
  19900 | Kerberos 5, etype 18, Pre-Auth                      | Network Protocol
   7500 | Kerberos 5, etype 23, AS-REQ Pre-Auth               | Network Protocol
  13100 | Kerberos 5, etype 23, TGS-REP                       | Network Protocol
  18200 | Kerberos 5, etype 23, AS-REP                        | Network Protocol
   5500 | NetNTLMv1 / NetNTLMv1+ESS                           | Network Protocol
  27000 | NetNTLMv1 / NetNTLMv1+ESS (NT)                      | Network Protocol
   5600 | NetNTLMv2                                           | Network Protocol
  27100 | NetNTLMv2 (NT)                                      | Network Protocol
   4800 | iSCSI CHAP authentication, MD5(CHAP)                | Network Protocol

```

Hashcat symbols:

```bash
  ?l = abcdefghijklmnopqrstuvwxyz
  ?u = ABCDEFGHIJKLMNOPQRSTUVWXYZ
  ?d = 0123456789
  ?s =  !"#$%&'()*+,-./:;<=>?@[\]^_`{|}~
  ?a = ?l?u?d?s
  ?b = 0x00 - 0xff
```

Hashcat attack modes (-a, --attack-mode):

```bash
  0 | Straight
  1 | Combination
  3 | Brute-force
  6 | Hybrid Wordlist + Mask
```

Hashcat workloads (-w, --workload-profile):

```bash
  # | Performance | Runtime | Power Consumption | Desktop Impact
 ===+=============+=========+===================+=================
  1 | Low         |   2 ms  | Low               | Minimal
  2 | Default     |  12 ms  | Economic          | Noticeable
  3 | High        |  96 ms  | High              | Unresponsive
  4 | Nightmare   | 480 ms  | Insane            | Headless

```

### Brute-force example

```bash
sudo hashcat -m 22000 -a 3 ./capture.hc22000 ?d?d?d?d?d?d?d?d?d # 8 digit numeric
```
