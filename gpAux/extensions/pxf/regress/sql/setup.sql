------------------------------------------------------------------
-- PXF Extension Creation
------------------------------------------------------------------

CREATE EXTENSION pxf;

CREATE EXTERNAL TABLE pxf_read_test (a INT, b TEXT)
LOCATION ('pxf://namenode:51200/data/pxf_hdfs_read.txt?PROFILE=TestProfile')
FORMAT 'TEXT' (DELIMITER ',');
