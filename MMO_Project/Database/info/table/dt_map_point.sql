CREATE TABLE [dbo].[dt_map_point]
(
	[point_no] INT NOT NULL,
	[map_no] INT NOT NULL,
	[type] TINYINT NOT NULL,
	[pos_x] FLOAT NOT NULL,
	[pos_y] FLOAT NOT NULL,
	[pos_z] FLOAT NOT NULL,
	[desc] NVARCHAR(200) NULL,
	CONSTRAINT pk_dt_map_point PRIMARY KEY CLUSTERED (point_no)
);
GO

CREATE NONCLUSTERED INDEX idx_dt_map_point_map_no
ON [dbo].[dt_map_point] ([map_no]);
GO

EXEC sys.sp_addextendedproperty
    @name = N'MS_Description',
    @value = N'포인트 종류 (common::map_point_type, 0 = pc_spawn)',
    @level0type = N'SCHEMA', @level0name = N'dbo',
    @level1type = N'TABLE', @level1name = N'dt_map_point',
    @level2type = N'COLUMN', @level2name = N'type';
GO

EXEC sys.sp_addextendedproperty
    @name = N'MS_Description',
    @value = N'언리얼 월드 좌표 (cm)',
    @level0type = N'SCHEMA', @level0name = N'dbo',
    @level1type = N'TABLE', @level1name = N'dt_map_point',
    @level2type = N'COLUMN', @level2name = N'pos_x';
GO
