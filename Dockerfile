# Multi-stage build for the ToC MUD server
FROM ubuntu:24.04 AS build
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
 && apt-get install -y --no-install-recommends build-essential make \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .
RUN make \
 && cp merc area/merc

FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /app/area
RUN apt-get update \
 && apt-get install -y --no-install-recommends libxcrypt1 \
 && rm -rf /var/lib/apt/lists/*

COPY --from=build /app /app
COPY --from=build /app/docker-entrypoint.sh /usr/local/bin/docker-entrypoint.sh
RUN chmod +x /usr/local/bin/docker-entrypoint.sh

EXPOSE 9000
ENV PORT=9000
ENTRYPOINT ["/usr/local/bin/docker-entrypoint.sh"]
CMD []
