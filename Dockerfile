FROM node:22-alpine3.21

ENV NODE_ENV=dev
ENV PORT=8000

ARG COMMIT_SHA
ARG REPO_NAME

ENV COMMIT_SHA=${COMMIT_SHA}
ENV REPO_NAME=${REPO_NAME}

RUN apk add curl aws-cli build-base jq openssl openssl-dev

RUN gcc --version
RUN g++ --version
RUN python3 --version

RUN openssl -version
ENV OPENSSL_ROOT=/usr/include/openssl

WORKDIR /usr/app
COPY ./.npmrc ./
COPY ./ ./
RUN npm install -g pnpm@9.10.0
RUN pnpm install

RUN jq --arg sha "$COMMIT_SHA" --arg repo "$REPO_NAME" \
'. + { "rtp": {"commitSha": $sha, "repoName": $repo} }' package.json > tmp.json && mv tmp.json package.json

RUN pnpm build-e2e

CMD ["node", "build/tests/e2e/index.js"]