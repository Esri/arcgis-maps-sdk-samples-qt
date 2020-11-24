# Copyright 2020 Esri.

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Using Debian slim which uses minimum packages necessary; Alpine is not preferred 
FROM alpine:3.12

LABEL maintainer="Tanner Yould <tyould@esri.com>"

# Setting PYTHONUNBUFFERED=TRUE or PYTHONUNBUFFERED=1 (they are equivalent) allows for log messages 
# to be immediately dumped to the stream instead of being buffered. This is useful for receiving timely 
# log messages and avoiding situations where the application crashes without emitting a relevant message 
# due to the message being "stuck" in a buffer.
ENV PYTHONUNBUFFERED=1

# Add scripts for the check
ADD entry.py /entry.py
ADD style.rb /style.rb

# Install dependencies
RUN echo "**** Install Ruby and mdl ****" && \
    apk add --update --no-cache ruby-full && \
    gem install mdl --no-document && \
    echo "**** Install Python ****" && \
    apk add --no-cache python3 && \
    if [ ! -e /usr/bin/python ]; then ln -sf python3 /usr/bin/python ; fi

# An ENTRYPOINT allows you to configure a container that will run as an executable.
# ex. ENTRYPOINT ["executable", "param1", "param2"]
ENTRYPOINT ["python3", "/entry.py"]
