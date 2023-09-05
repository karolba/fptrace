# My fork of `fptrace` with modifications intended to be upstreamed

Included on this `master` branch:
- Port to `arm64` - https://github.com/orivej/fptrace/pull/6
- A performance improvement: don't trace open/write/read syscalls if `-d` isn't specified and don't trace unlink if `-u` isn't specified either - waiting on the previous PR to get reviewed & accepted until this is submitted
- Show usage if no no-flags arguments are specified
