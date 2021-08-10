Here is how to test rcppsimdjson.

- Install docker.
- Grab rcppsimdjson from GitHub.

Next, you want to create a file called 'Dockerfile'. You may put this file inside the root rcppsimdjson repository.

The file may have the following content:

```
 FROM ubuntu:18.04
 ARG USER_NAME
 ARG USER_ID
 ARG GROUP_ID
 # We install some useful packages
 RUN apt-get update -qq
 RUN apt-get install software-properties-common -qq
 RUN DEBIAN_FRONTEND="noninteractive" apt-get -y install tzdata g++ gcc
 RUN add-apt-repository -y ppa:c2d4u.team/c2d4u4.0+
 RUN apt-get -q update
 RUN apt-get install curl sudo -qq
 RUN curl -OLs https://eddelbuettel.github.io/r-ci/run.sh && chmod 0755 run.sh
 RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test
 RUN ./run.sh install_aptget r-cran-rcpp r-cran-bit64 r-cran-tinytest
 # setting the environment
 RUN addgroup --gid $GROUP_ID user; exit 0
 RUN adduser --disabled-password --gecos '' --uid $USER_ID --gid $GROUP_ID $USER_NAME; exit 0
 RUN echo "$USER_NAME:$USER_NAME" | chpasswd && adduser $USER_NAME sudo
 RUN echo '----->'
 RUN echo 'root:Docker!' | chpasswd
 ENV TERM xterm-256color
 USER $USER_NAME
```

You may create the container (once), as follows:

```
docker build --no-cache -f Dockerfile -t rcppsimdjson  --build-arg USER_NAME="$USER"  --build-arg USER_ID=$(id -u)  --build-arg GROUP_ID=$(id -g) .
```

Next, while being in the rcppsimdjson repository, you may run the following:

```
docker run -it -v $(pwd):$(pwd):Z  -w $(pwd)  rcppsimdjson  bash
```

This will start a bash shell. Inside the bash shell, you may want to issue
the following commands (note that your password is your username inside the container):

```
./run.sh bootstrap
./run.sh run_tests
```

