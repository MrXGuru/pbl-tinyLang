# Use a slim Python image to keep the container small
FROM python:3.11-slim

# Install system dependencies needed for compiling the C backend
# gcc, make, and libc-dev are required for the Makefile to work
RUN apt-get update && \
    apt-get install -y --no-install-recommends gcc make libc-dev && \
    rm -rf /var/lib/apt/lists/*

# Set the working directory in the container
WORKDIR /app

# Copy the dependency file first to leverage Docker cache
COPY requirements.txt .

# Install Python dependencies
RUN pip install --no-cache-dir -r requirements.txt

# Copy the rest of the application code
COPY . .

# Compile the C compiler core
# This creates the tinylang_compiler binary needed by the Flask backend
RUN make

# Set environment variables
ENV PORT=5000
ENV FLASK_ENV=production
ENV PYTHONUNBUFFERED=1

# Expose the port the app runs on
EXPOSE 5000

# Command to run the application using Gunicorn (production WSGI server)
# It binds to 0.0.0.0 and dynamically reads the PORT environment variable
CMD gunicorn -w 4 -b 0.0.0.0:${PORT:-5000} app:app
