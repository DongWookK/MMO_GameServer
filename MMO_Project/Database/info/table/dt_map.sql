CREATE TABLE [dbo].[dt_map]
(
	[map_no] INT NOT NULL ,
    [map_name] NVARCHAR(50) NOT NULL,
    [type] TINYINT NOT NULL,
	[filename] NVARCHAR(200) NULL,
    [desc] NVARCHAR(200) NULL,
	CONSTRAINT pk_dt_map PRIMARY KEY CLUSTERED (map_no)
);
GO;

EXEC sys.sp_addextendedproperty 
    @name = N'MS_Description', 
    @value = N'맵 번호 (PK)', 
    @level0type = N'SCHEMA', @level0name = N'dbo', 
    @level1type = N'TABLE', @level1name = N'dt_map', 
    @level2type = N'COLUMN', @level2name = N'map_no';