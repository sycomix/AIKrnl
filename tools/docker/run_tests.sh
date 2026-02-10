#!/usr/bin/env bash
set -euo pipefail

IMAGE_NAME=aios-test:latest

echo "Building $IMAGE_NAME..."
docker build -t "$IMAGE_NAME" .

echo "Running tests in container..."
docker run --rm "$IMAGE_NAME"

echo "Done. Container tests finished."