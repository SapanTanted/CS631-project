SET client_encoding = 'UTF8';
--
-- Name: client_table; Type: TABLE; Schema: public; Owner: seil
--

CREATE TABLE public.client_table (
    connection_timestamp timestamp without time zone,
    client_id character varying(100) NOT NULL,
    number_of_subscriptions integer
);


ALTER TABLE public.client_table OWNER TO seil;

--
-- Name: topic_table; Type: TABLE; Schema: public; Owner: seil
--

CREATE TABLE public.topic_table (
    topic_id serial NOT NULL,
    topic character varying(100) UNIQUE,
    last_msg_recv_timestamp timestamp without time zone,
    relative_timeout bigint DEFAULT 0
);


ALTER TABLE public.topic_table OWNER TO seil;
--
-- Name: client_table_pkey; Type: CONSTRAINT; Schema: public; Owner: seil
--

ALTER TABLE ONLY public.client_table
    ADD CONSTRAINT client_table_pkey PRIMARY KEY (client_id);


--
-- Name: topic_table_pkey; Type: CONSTRAINT; Schema: public; Owner: seil
--

ALTER TABLE ONLY public.topic_table
    ADD CONSTRAINT topic_table_pkey PRIMARY KEY (topic_id);



create table payload_table (client_id varchar(100), topic_id int, payload varchar(2000), payload_timestamp timestamp, expiry_timestamp timestamp);

create table subscription_table (client_id varchar(100), topic_id int, subscription_timestamp timestamp, timeout bigint, last_ping_timestamp timestamp, primary key(client_id,topic_id)) ;
--
-- Name: SCHEMA public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;
--
-- PostgreSQL database dump complete
--

