#!/bin/bash
echo "Installing Python dependencies..."
pip install -r requirements.txt

echo "Building C Compiler..."
make
