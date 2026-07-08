FROM php:8.2-apache

# Enable Apache mod_rewrite
RUN a2enmod rewrite

# Copy API files
COPY keysystem/api/ /var/www/html/

# Set permissions for SQLite
RUN chown -R www-data:www-data /var/www/html && chmod 777 /var/www/html

# Create empty database file
RUN touch /var/www/html/database.sqlite && chown www-data:www-data /var/www/html/database.sqlite && chmod 666 /var/www/html/database.sqlite
