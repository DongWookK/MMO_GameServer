CREATE TABLE [dbo].[tb_pc]
(
    [pc_no]         BIGINT NOT NULL IDENTITY(1,1),
	[user_no]       INT NOT NULL,
    [pc_name]       NVARCHAR(20) NOT NULL,
    [pc_type]       TINYINT NOT NULL,
	[level]         INT NOT NULL,
	[exp]           BIGINT NOT NULL,
    [hp]            INT NOT NULL, 
    [mp]            INT NOT NULL,
    [map_no]        INT NOT NULL DEFAULT(0),
    [location_x]    FLOAT NOT NULL DEFAULT(0),
    [location_y]    FLOAT NOT NULL DEFAULT(0),
    [location_z]    FLOAT NOT NULL DEFAULT(0),
	CONSTRAINT pk_tb_pc PRIMARY KEY CLUSTERED (pc_no)
);
GO;

CREATE NONCLUSTERED INDEX idx_tb_pc_user_no
ON [dbo].[tb_pc] ([user_no]);
GO

CREATE UNIQUE NONCLUSTERED INDEX uix_tb_pc_pc_name
ON [dbo].[tb_pc] ([pc_name]);
GO

EXEC sys.sp_addextendedproperty 
    @name = N'MS_Description', 
    @value = N'캐릭터 고유 ID (PK)', 
    @level0type = N'SCHEMA', @level0name = N'dbo', 
    @level1type = N'TABLE', @level1name = N'tb_pc', 
    @level2type = N'COLUMN', @level2name = N'pc_no';
GO;

EXEC sys.sp_addextendedproperty 
    @name = N'MS_Description', 
    @value = N'캐릭터 클래스', 
    @level0type = N'SCHEMA', @level0name = N'dbo', 
    @level1type = N'TABLE', @level1name = N'tb_pc', 
    @level2type = N'COLUMN', @level2name = N'pc_type';
GO;

EXEC sys.sp_addextendedproperty
    @name = N'MS_Description',
    @value = N'현재 맵 번호 (0 = 없음, 서버가 기본 맵 스폰 포인트로 입장시킨다)',
    @level0type = N'SCHEMA', @level0name = N'dbo',
    @level1type = N'TABLE', @level1name = N'tb_pc',
    @level2type = N'COLUMN', @level2name = N'map_no';
GO;