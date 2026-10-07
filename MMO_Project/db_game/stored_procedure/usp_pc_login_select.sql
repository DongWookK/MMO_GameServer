/* ------------------------------------------------------------
title   : usp_pc_login_select
desc    : 캐릭터 선택, 게임 접속

2026.10.05 | 김동욱 | 최초 생성
2026.10.08 | 김동욱 | map_no 추가
*/ -----------------------------------------------------------
CREATE PROCEDURE [dbo].[usp_pc_login_select]
    @p_user_no  INT
    , @p_pc_no  BIGINT
AS
BEGIN
    SET NOCOUNT ON;
    SET XACT_ABORT ON;
    
    SELECT pc_name
        , pc_type
        , level
        , exp
        , hp
        , mp
        , map_no
        , location_x
        , location_y
        , location_z
    FROM [dbo].[tb_pc]
    WHERE user_no = @p_user_no
        AND pc_no = @p_pc_no
END
GO