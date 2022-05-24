#!/bin/bash

echo "GET / HTTP/1.1" | nc 0.0.0.0 4242
echo "Host: localhost" | nc 0.0.0.0 4242
echo "Connection: Keep-Alive" | nc 0.0.0.0 4242
echo "Referer: https://www.google.com/" | nc 0.0.0.0 4242
