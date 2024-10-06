FROM ubuntu:jammy

ARG DB_PASSWORD=libsese
ENV TERM=xterm-256color
ENV DEBIAN_FRONTEND=noninteractive

# 安装系统软件包（系统级操作）
# COPY ./scripts/install_ubuntu_deps.sh /tmp/install_deps.sh

# RUN yes | unminimize && \
#     apt update -y && \
#     apt upgrade -y && \
#     apt install -y --no-install-recommends apt-utils crudini doxygen python3-pip sqlite3 mariadb-client mariadb-server postgresql && \
#     bash ./tmp/install_deps.sh && \
#     apt autoremove -y && \
#     rm -rf /var/lib/apt/lists/*

RUN yes | unminimize && \
#    bash ./tmp/install_deps.sh && \
    apt install -y --no-install-recommends curl zip unzip tar git mysql-client postgresql-client sqlite3 python3-pip doxygen && \
    apt autoremove -y && \
    rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/microsoft/vcpkg /opt/vcpkg && \
    /opt/vcpkg/bootstrap-vcpkg.sh -disableMetrics

ENV VCPKG_ROOT=/opt/vcpkg

# 安装 pip 软件包（非系统级操作）
COPY ./requirements.txt /tmp/requirements.txt

RUN pip install -r /tmp/requirements.txt && \
    pip cache purge

# 初始化数据库（非系统级操作）
# COPY ./scripts/*.sql /tmp/

# RUN sqlite3 /tmp/db_test.db < /tmp/sqlite_dump.sql

# RUN service mysql start && \
#     mysqladmin -u root password '${DB_PASSWORD}' && \
#     crudini --set /etc/mysql/debian.cnf client user root && \
#     crudini --set /etc/mysql/debian.cnf client password ${DB_PASSWORD} && \
#     crudini --set /etc/mysql/debian.cnf mysql_upgrade user root && \
#     crudini --set /etc/mysql/debian.cnf mysql_upgrade password ${DB_PASSWORD} && \
#     mysql -u root -p${DB_PASSWORD} -e "ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY '${DB_PASSWORD}';" && \
#     mysql -u root -p${DB_PASSWORD} < /tmp/mysql_dump.sql && \
#     service mysql stop

# RUN service mariadb start && \
#     mysqladmin -u root password '${DB_PASSWORD}' && \
#     crudini --set /etc/mysql/debian.cnf client user root && \
#     crudini --set /etc/mysql/debian.cnf client password ${DB_PASSWORD} && \
#     crudini --set /etc/mysql/debian.cnf mysql_upgrade user root && \
#     crudini --set /etc/mysql/debian.cnf mysql_upgrade password ${DB_PASSWORD} && \
#     mysql -p${DB_PASSWORD} -e "ALTER USER 'root'@'localhost' IDENTIFIED BY '${DB_PASSWORD}';" && \
#     mysql -p${DB_PASSWORD} < /tmp/mysql_dump.sql && \
#     service mariadb stop

# RUN service postgresql start && \
#     su - postgres -c "psql -c \"CREATE ROLE root superuser PASSWORD '${DB_PASSWORD}' login;\"" && \
#     psql -U root -d postgres -f /tmp/postgres_dump.sql && \
#     service postgresql stop