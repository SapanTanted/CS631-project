make install
psql postgres -c "drop extension pub_sub_extension;"
psql postgres -c "create extension pub_sub_extension;"
psql postgres
