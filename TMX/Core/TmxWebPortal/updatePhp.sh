#!/bin/bash
#php update script

echo "downloading pear"
apt-get install php-pear
apt-get install libpcre3-dev
pecl install zip

cp php.ini /etc/php5/apache2/php.ini

mkdir /var/www/plugins
chmod 755 /var/www/plugins