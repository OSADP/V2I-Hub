#!/bin/sh -e
chown -h root:root /usr/local/lib/libmysqlcppconn.so*

mkdir -p /var/log/tmx
chmod 755 /var/log/tmx

mkdir -p /var/www/plugins
chmod 755 /var/www/plugins

set +e
id plugin >/dev/null 2>&1
if [ $? -ne 0 ]; then
	adduser --system --disabled-login --disabled-password --gecos --no-create-home plugin
fi

usermod -a -G dialout plugin
set -e

cp -r ../TMX/TmxWebPortal /var/www/tmx

echo "<?PHP header(\"location:tmx\") ?>" > /var/www/index.php
chmod 644 /var/www/index.php
chown www-data:www-data /var/www/*
chown -R www-data:www-data /var/www/tmx
chmod 755 /var/www/tmx

sed '/short_open_tag/s/Off/On/g' -i /etc/php5/apache2/php.ini
rm -f /etc/apache2/sites-enabled/*
ln -s /etc/apache2/sites-available/default-site /etc/apache2/sites-enabled/default-site

set +e
php5enmod mcrypt
#pecl install zip
set -e

initctl reload-configuration
service apache2 restart

