CREATE TABLE [dbo].[tb_server_dsn]
(
	[server_no] SMALLINT NOT NULL,
	[type] TINYINT NOT NULL,
	[dsn] VARCHAR(50) NOT NULL,
	CONSTRAINT pk_tb_server_dsn PRIMARY KEY CLUSTERED (server_no, type)
)
GO;

EXEC sys.sp_addextendedproperty 
    @name = N'MS_Description', 
    @value = N'서버 번호 (PK)', 
    @level0type = N'SCHEMA', @level0name = N'dbo', 
    @level1type = N'TABLE', @level1name = N'tb_server_dsn', 
    @level2type = N'COLUMN', @level2name = N'server_no';

GO;
    EXEC sys.sp_addextendedproperty 
    @name = N'MS_Description', 
    @value = N'dsn 타입', 
    @level0type = N'SCHEMA', @level0name = N'dbo', 
    @level1type = N'TABLE', @level1name = N'tb_server_dsn', 
    @level2type = N'COLUMN', @level2name = N'type';