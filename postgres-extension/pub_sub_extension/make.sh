sudo make install
psql postgres -c "drop extension pub_sub_extension;create extension pub_sub_extension;"
psql postgres