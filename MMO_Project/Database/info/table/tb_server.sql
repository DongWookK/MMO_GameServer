CREATE TABLE [dbo].[tb_server]
(
	[server_no] INT NOT NULL ,
    [type] TINYINT NOT NULL,
	[desc] NVARCHAR(20) NULL,
    [ip] NVARCHAR(20) NOT NULL,
	CONSTRAINT pk_tb_server PRIMARY KEY CLUSTERED (server_no)
);
GO;

EXEC sys.sp_addextendedproperty 
    @name = N'MS_Description', 
    @value = N'서버 번호 (PK)', 
    @level0type = N'SCHEMA', @level0name = N'dbo', 
    @level1type = N'TABLE', @level1name = N'tb_server', 
    @level2type = N'COLUMN', @level2name = N'server_no';