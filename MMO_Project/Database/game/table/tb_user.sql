CREATE TABLE [dbo].[tb_user]
(
	[user_no] INT NOT NULL IDENTITY(0,0),
    [user_name] NVARCHAR(20) NOT NULL,
	[login_time] DATETIME2 NULL,
	[logout_time] DATETIME2 NULL,
	CONSTRAINT pk_tb_user PRIMARY KEY CLUSTERED (user_no)
);
GO;

EXEC sys.sp_addextendedproperty 
    @name = N'MS_Description', 
    @value = N'사용자 고유 ID (PK)', 
    @level0type = N'SCHEMA', @level0name = N'dbo', 
    @level1type = N'TABLE', @level1name = N'tb_user', 
    @level2type = N'COLUMN', @level2name = N'user_no';