FROM ubuntu:bionic

# App Sepcific
ARG XI_APP
ENV XI_APP ${XI_APP}
CMD /bin/${XI_APP}
COPY ./${XI_APP} /bin

VOLUME [ "/data" ]
WORKDIR /data

ENV XI_NETWORK_DIR=/data/network
ENV XI_BREAKPAD_OUT /data/breakpad
ENV XI_SSL_DIR /data/ssl
ENV XI_DATA_DIR /data/data
