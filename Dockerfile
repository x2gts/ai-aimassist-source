FROM php:8.2-apache

# Enable Apache mod_rewrite
RUN a2enmod rewrite

# Copy API files
COPY keysystem/api/ /var/www/html/

# Set permissions for SQLite
RUN chown -R www-data:www-data /var/www/html && chmod 777 /var/www/html

# Create persistent data directory for SQLite
RUN mkdir -p /var/data && chown www-data:www-data /var/data

# Create database on persistent disk (won't be lost on redeploy)
RUN touch /var/data/database.sqlite && chown www-data:www-data /var/data/database.sqlite && chmod 666 /var/data/database.sqlite

# Symlink so the API can find it at the expected path
RUN ln -sf /var/data/database.sqlite /var/www/html/database.sqlite
