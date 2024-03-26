FROM archlinux:base-devel
COPY . /
RUN pacman -Syy git lcov gtest perl-json-xs --noconfirm
RUN curl -L https://coveralls.io/coveralls-linux.tar.gz | tar -xz -C /usr/local/bin
RUN make coverage
CMD coveralls report tests/build/coverage.info --format=lcov
