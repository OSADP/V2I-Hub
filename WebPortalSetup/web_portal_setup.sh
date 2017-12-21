#!/bin/sh -e

mkdir -p /var/log/tmx
chmod 755 /var/log/tmx

mkdir -p /var/www/plugins
chmod 755 /var/www/plugins

if [ -d "/usr/local/BsmReceiver" ]; then
mv /usr/local/BsmReceiver/ /var/www/plugins/
fi

if [ -d "/usr/local/CommandPlugin" ]; then
mv /usr/local/CommandPlugin/ /var/www/plugins/
fi

if [ -d "/usr/local/CSW" ]; then
mv /usr/local/CSW/ /var/www/plugins/
fi

if [ -d "/usr/local/DSRCMessageManager" ] ; then
mv /usr/local/DSRCMessageManager/ /var/www/plugins/
fi

if [ -d "/usr/local/DynamicMessageSign" ]; then
mv /usr/local/DynamicMessageSign/ /var/www/plugins/
fi

if [ -d "/usr/local/Location" ]; then
mv /usr/local/Location/ /var/www/plugins/
fi

if [ -d "/usr/local/MAP" ]; then
mv /usr/local/MAP/ /var/www/plugins/
fi

if [ -d "/usr/local/ODEPlugin" ]; then
mv /usr/local/ODEPlugin/ /var/www/plugins/
fi

if [ -d "/usr/local/SPAT" ]; then
mv /usr/local/SPAT/ /var/www/plugins/
fi

if [ -d "/usr/local/UIProxyPlugin" ]; then
mv /usr/local/UIProxyPlugin/ /var/www/plugins/
fi

sudo chown -R www-data /var/www/plugins/*

if [ ! -d "/var/www/plugins/.ssl" ]; then
mkdir /var/www/plugins/.ssl
chown www-data /var/www/plugins/.ssl
chgrp www-data /var/www/plugins/.ssl

echo "Creating SSL CERT"

sudo openssl req -x509 -newkey rsa:4096 -sha256 -nodes -keyout /var/www/plugins/.ssl/tmxcmd.key -out /var/www/plugins/.ssl/tmxcmd.crt -subj "/CN=battelle.com" -days 3650
fi

chown www-data *
chgrp www-data *

echo "Creating Installing Plugins"

tmxctl --load-manifest /var/www/plugins/BsmReceiver/manifest.json
tmxctl --load-manifest /var/www/plugins/CommandPlugin/manifest.json
tmxctl --load-manifest /var/www/plugins/CSW/manifest.json
tmxctl --load-manifest /var/www/plugins/DSRCMessageManager/manifest.json
tmxctl --load-manifest /var/www/plugins/DynamicMessageSign/manifest.json
tmxctl --load-manifest /var/www/plugins/Location/manifest.json
tmxctl --load-manifest /var/www/plugins/MAP/manifest.json
tmxctl --load-manifest /var/www/plugins/ODEPlugin/manifest.json
tmxctl --load-manifest /var/www/plugins/SPAT/manifest.json
tmxctl --load-manifest /var/www/plugins/UIProxyPlugin/manifest.json


echo "Configuring Apache2"

cp v2ihub.conf /etc/apache2/sites-available/
rm /etc/apache2/sites-enabled/*
ln -s /etc/apache2/sites-available/v2ihub.conf /etc/apache2/sites-enabled/v2ihub.conf
systemctl reload apache2

echo "Adding Plugin User"

set +e
id plugin >/dev/null 2>&1
if [ $? -ne 0 ]; then
	adduser --system --disabled-login --disabled-password --gecos --no-create-home plugin
fi

usermod -a -G dialout plugin
set -e

echo "Updating Permissions"

chown www-data:www-data /var/www/*
chown -R www-data:www-data /var/www/v2ihub
chmod 755 /var/www/v2ihub

echo "Enabling Command Plugin"

tmxctl --enable CommandPlugin



