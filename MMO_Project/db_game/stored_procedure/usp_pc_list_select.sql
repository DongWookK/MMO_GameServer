/* ------------------------------------------------------------
title   : usp_pc_list_select
desc    : 캐릭터 목록 조회 (캐릭터 선택 화면)

2026.10.05 | 김동욱 | 최초 생성
*/ -----------------------------------------------------------
CREATE PROCEDURE [dbo].[usp_pc_list_select]
    @p_user_no  INT
AS
BEGIN
    SET NOCOUNT ON;

    SELECT pc_no
        , pc_name
        , pc_type
        , level
    FROM [dbo].[tb_pc]
    WHERE user_no = @p_user_no
    ORDER BY pc_no
END
GO
