FROM ubuntu:24.04

RUN apt-get update && \
    apt-get install -y python3 python3-pip python3.12-venv && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /api

RUN python3 -m venv venv

# Install dependencies
COPY requirements.txt .
RUN venv/bin/pip install --no-cache-dir -r requirements.txt

COPY ./entrypoints ./entrypoints
RUN chmod +x ./entrypoints/*
