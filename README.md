Downgrade attacks
======

This repo was modified from the original [SrsRAN Project](https://github.com/srsran/srsRAN) at commit [89f16ee](https://github.com/jwijenbergh/srsran-downgrade/commit/89f16eed2b7f2e6c45e675298f5829c230698e1d) with the following modifications:
- Branch [downgrade-attach-reject](https://github.com/jwijenbergh/srsran-downgrade/tree/downgrade-attach-reject): Send an Attach Reject upon Attach Request with Cause *EPS Services Not Allowed*
- Branch [downgrade-service-reject](https://github.com/jwijenbergh/srsran-downgrade/tree/downgrade-service-reject): Send a Service Reject upon Service Request with Cause *EPS Services Not Allowed*
- Branch [dos-attach-reject](https://github.com/jwijenbergh/srsran-downgrade/tree/dos-attach-reject): Send an Attach Reject upon Attach Request with Cause *EPS Services And Non EPS Services Not Allowed*
- Branch [dos-service-reject](https://github.com/jwijenbergh/srsran-downgrade/tree/dos-service-reject): Send a Service Reject upon Service Request with Cause *EPS Services And Non EPS Services Not Allowed*

srsRAN
======

[![Build Status](https://travis-ci.com/srsran/srsRAN.svg?branch=master)](https://travis-ci.com/srsran/srsRAN)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/srsran/srsRAN.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/srsran/srsRAN/context:cpp)
[![Coverity](https://scan.coverity.com/projects/23048/badge.svg)](https://scan.coverity.com/projects/srsran_agpl)

srsRAN is a 4G/5G software radio suite developed by [SRS](http://www.srs.io).

See the [srsRAN project pages](https://www.srsran.com) for information, guides and project news.

The srsRAN suite includes:
  * srsUE - a full-stack SDR 4G/5G-NSA UE application (5G-SA coming soon)
  * srsENB - a full-stack SDR 4G eNodeB application (5G-NSA and 5G-SA coming soon)
  * srsEPC - a light-weight 4G core network implementation with MME, HSS and S/P-GW

For application features, build instructions and user guides see the [srsRAN documentation](https://docs.srsran.com).

For license details, see LICENSE file.

Support
=======

Mailing list: https://lists.softwareradiosystems.com/mailman/listinfo/srslte-users
