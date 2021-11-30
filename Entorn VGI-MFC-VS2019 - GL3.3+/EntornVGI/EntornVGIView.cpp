//******** PRACTICA VISUALITZACI� GR�FICA INTERACTIVA (Escola Enginyeria - UAB)
//******** Entorn b�sic VS2019 MULTIFINESTRA amb OpenGL 3.3+, interf�cie MFC, llibreries GLM i primitives GLUT en codi font
//******** Ferran Poveda, Marc Vivet, Carme Juli�, D�bora Gil, Enric Mart� (Setembre 2021)
// EntornVGIView.cpp: implementaci�n de la clase CEntornVGIView
// FUNCIONS:		- Control del bucle principal (OnPaint)
//					- Control teclat (OnKeyDown)
//					- Control mouse interactiu i botons mouse 
//							(OnLButtomDown, OnRButtomDown, OnMouseMove)
//					- Control opcions de men� (On*, OnUpdate*)
//					- Control de color de fons per teclat (FONS)
//					- Transformacions Geom�triques Interactives via mouse
//
//    Versi� 2.0:	- Canvi de color de l'objecte per teclat (OBJ)
//					- Fixar Transformacions Geom�triques desactivant opcions Transforma (INSERT dins opcions Transforma)
//					- Esborrar Transformacions Geom�triques fixades (DELETE dins opcions Transforma)
//					- Fixar Traslaci� pantalla fora de l'opci� Vista->Pan? (INSERT dins opci� Vista->Pan?)
//					- Esborrar Traslaci� pantalla fixada (DELETE dins opci� Vista->Pan?)
//
//	  Versi� 2.2:	- Opci� VISTA --> Sat�l.lit: Punt de Vista en moviment segons moviment mouse
//
//	  Versi� 2.5:	- Afegits objectes cubRGB i Tie (nau Star Wars fet per alumnes)
//					- Opci� VISTA --> FullScreen?: Opci� FullScreen per men� i per PopUp menu enmig pantalla. T� un bug al restaurar pantalla 
//													normal (inclos men� IDR_MENU_WINDOW)
//	  Versi� 2.6:	- Afegides funcions per a displaiar texte (PrintText)
//

#include "stdafx.h"

// Se pueden definir SHARED_HANDLERS en un proyecto ATL implementando controladores de vista previa, miniatura
// y filtro de b�squeda, y permiten compartir c�digo de documentos con ese proyecto.
#ifndef SHARED_HANDLERS
#include "EntornVGI.h"
#endif

#include "EntornVGIDoc.h"
#include "EntornVGIView.h"
#include "visualitzacio.h"	// Include funcions de projeci� i il.luminaci�
#include "escena.h"			// Include funcions d'objectes OpenGL

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "Estructura-de-datos/Piece.h"

Piece pieces[5];
Block vaoBlocks[5];
Board m_board;
int numPiece;
/////////////////////////////////////////////////////////////////////////////
// CEntornVGIView

IMPLEMENT_DYNCREATE(CEntornVGIView, CView)

BEGIN_MESSAGE_MAP(CEntornVGIView, CView)
	// Comandos de impresi�n est�ndar
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CEntornVGIView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_COMMAND(ID_ARXIU_OBRIR_FRACTAL, &CEntornVGIView::OnArxiuObrirFractal)
	ON_COMMAND(ID_ARXIU_OBRIR_FITXER_OBJ, &CEntornVGIView::OnArxiuObrirFitxerObj)
	ON_COMMAND(ID_ARXIU_OBRIR_FITXER_3DS, &CEntornVGIView::OnArxiuObrirFitxer3ds)
	ON_COMMAND(ID_VISTA_Mobil, &CEntornVGIView::OnVistaMobil)
	ON_UPDATE_COMMAND_UI(ID_VISTA_Mobil, &CEntornVGIView::OnUpdateVistaMobil)
	ON_COMMAND(ID_VISTA_ZOOM, &CEntornVGIView::OnVistaZoom)
	ON_UPDATE_COMMAND_UI(ID_VISTA_ZOOM, &CEntornVGIView::OnUpdateVistaZoom)
	ON_COMMAND(ID_VISTA_POLARS_X, &CEntornVGIView::OnVistaPolarsX)
	ON_UPDATE_COMMAND_UI(ID_VISTA_POLARS_X, &CEntornVGIView::OnUpdateVistaPolarsX)
	ON_COMMAND(ID_VISTA_POLARS_Y, &CEntornVGIView::OnVistaPolarsY)
	ON_UPDATE_COMMAND_UI(ID_VISTA_POLARS_Y, &CEntornVGIView::OnUpdateVistaPolarsY)
	ON_COMMAND(ID_VISTA_POLARS_Z, &CEntornVGIView::OnVistaPolarsZ)
	ON_UPDATE_COMMAND_UI(ID_VISTA_POLARS_Z, &CEntornVGIView::OnUpdateVistaPolarsZ)
	ON_COMMAND(ID_VISTA_PAN, &CEntornVGIView::OnVistaPan)
	ON_UPDATE_COMMAND_UI(ID_VISTA_PAN, &CEntornVGIView::OnUpdateVistaPan)
	ON_COMMAND(ID_VISTA_ORIGENPAN, &CEntornVGIView::OnVistaOrigenpan)
	ON_COMMAND(ID_VISTA_NAVEGA, &CEntornVGIView::OnVistaNavega)
	ON_UPDATE_COMMAND_UI(ID_VISTA_NAVEGA, &CEntornVGIView::OnUpdateVistaNavega)
	ON_COMMAND(ID_VISTA_ORIGENNAVEGA, &CEntornVGIView::OnVistaOrigennavega)
	ON_COMMAND(ID_VISTA_EIXOS, &CEntornVGIView::OnVistaEixos)
	ON_UPDATE_COMMAND_UI(ID_VISTA_EIXOS, &CEntornVGIView::OnUpdateVistaEixos)
	ON_COMMAND(ID_PROJECCIO_PERSPECTIVA, &CEntornVGIView::OnProjeccioPerspectiva)
	ON_UPDATE_COMMAND_UI(ID_PROJECCIO_PERSPECTIVA, &CEntornVGIView::OnUpdateProjeccioPerspectiva)
	ON_COMMAND(ID_OBJECTE_CUB, &CEntornVGIView::OnObjecteCub)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_CUB, &CEntornVGIView::OnUpdateObjecteCub)
	ON_COMMAND(ID_OBJECTE_ESFERA, &CEntornVGIView::OnObjecteEsfera)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_ESFERA, &CEntornVGIView::OnUpdateObjecteEsfera)
	ON_COMMAND(ID_OBJECTE_TETERA, &CEntornVGIView::OnObjecteTetera)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_TETERA, &CEntornVGIView::OnUpdateObjecteTetera)
	ON_COMMAND(ID_TRANSFORMA_TRASLACIO, &CEntornVGIView::OnTransformaTraslacio)
	ON_UPDATE_COMMAND_UI(ID_TRANSFORMA_TRASLACIO, &CEntornVGIView::OnUpdateTransformaTraslacio)
	ON_COMMAND(ID_TRANSFORMA_ORIGENTRASLACIO, &CEntornVGIView::OnTransformaOrigentraslacio)
	ON_COMMAND(ID_TRANSFORMA_ROTACIO, &CEntornVGIView::OnTransformaRotacio)
	ON_UPDATE_COMMAND_UI(ID_TRANSFORMA_ROTACIO, &CEntornVGIView::OnUpdateTransformaRotacio)
	ON_COMMAND(ID_TRANSFORMA_ORIGENROTACIO, &CEntornVGIView::OnTransformaOrigenrotacio)
	ON_COMMAND(ID_TRANSFORMA_ESCALAT, &CEntornVGIView::OnTransformaEscalat)
	ON_UPDATE_COMMAND_UI(ID_TRANSFORMA_ESCALAT, &CEntornVGIView::OnUpdateTransformaEscalat)
	ON_COMMAND(ID_TRANSFORMA_ORIGENESCALAT, &CEntornVGIView::OnTransformaOrigenescalat)
	ON_COMMAND(ID_TRANSFORMA_MOBILX, &CEntornVGIView::OnTransformaMobilx)
	ON_UPDATE_COMMAND_UI(ID_TRANSFORMA_MOBILX, &CEntornVGIView::OnUpdateTransformaMobilx)
	ON_COMMAND(ID_TRANSFORMA_MOBILY, &CEntornVGIView::OnTransformaMobily)
	ON_UPDATE_COMMAND_UI(ID_TRANSFORMA_MOBILY, &CEntornVGIView::OnUpdateTransformaMobily)
	ON_COMMAND(ID_TRANSFORMA_MOBILZ, &CEntornVGIView::OnTransformaMobilz)
	ON_UPDATE_COMMAND_UI(ID_TRANSFORMA_MOBILZ, &CEntornVGIView::OnUpdateTransformaMobilz)
	ON_COMMAND(ID_OCULTACIONS_TESTVIS, &CEntornVGIView::OnOcultacionsTestvis)
	ON_UPDATE_COMMAND_UI(ID_OCULTACIONS_TESTVIS, &CEntornVGIView::OnUpdateOcultacionsTestvis)
	ON_COMMAND(ID_OCULTACIONS_ZBUFFER, &CEntornVGIView::OnOcultacionsZbuffer)
	ON_UPDATE_COMMAND_UI(ID_OCULTACIONS_ZBUFFER, &CEntornVGIView::OnUpdateOcultacionsZbuffer)
	ON_COMMAND(ID_OCULTACIONS_BACKLINE, &CEntornVGIView::OnOcultacionsBackline)
	ON_UPDATE_COMMAND_UI(ID_OCULTACIONS_BACKLINE, &CEntornVGIView::OnUpdateOcultacionsBackline)
	ON_COMMAND(ID_ILUMINACIO_LLUMFIXE, &CEntornVGIView::OnIluminacioLlumfixe)
	ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_LLUMFIXE, &CEntornVGIView::OnUpdateIluminacioLlumfixe)
	ON_COMMAND(ID_ILUMINACIO_FILFERROS, &CEntornVGIView::OnIluminacioFilferros)
	ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_FILFERROS, &CEntornVGIView::OnUpdateIluminacioFilferros)
	ON_COMMAND(ID_ILUMINACIO_PLANA, &CEntornVGIView::OnIluminacioPlana)
	ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_PLANA, &CEntornVGIView::OnUpdateIluminacioPlana)
	ON_COMMAND(ID_ILUMINACIO_GOURAUD, &CEntornVGIView::OnIluminacioGouraud)
	ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_GOURAUD, &CEntornVGIView::OnUpdateIluminacioGouraud)
	ON_COMMAND(ID_MATERIAL_EMISSIO, &CEntornVGIView::OnMaterialEmissio)
	ON_UPDATE_COMMAND_UI(ID_MATERIAL_EMISSIO, &CEntornVGIView::OnUpdateMaterialEmissio)
	ON_COMMAND(ID_MATERIAL_AMBIENT, &CEntornVGIView::OnMaterialAmbient)
	ON_UPDATE_COMMAND_UI(ID_MATERIAL_AMBIENT, &CEntornVGIView::OnUpdateMaterialAmbient)
	ON_COMMAND(ID_MATERIAL_DIFUSA, &CEntornVGIView::OnMaterialDifusa)
	ON_UPDATE_COMMAND_UI(ID_MATERIAL_DIFUSA, &CEntornVGIView::OnUpdateMaterialDifusa)
	ON_COMMAND(ID_MATERIAL_ESPECULAR, &CEntornVGIView::OnMaterialEspecular)
	ON_UPDATE_COMMAND_UI(ID_MATERIAL_ESPECULAR, &CEntornVGIView::OnUpdateMaterialEspecular)
	ON_COMMAND(ID_ILUMINACIO_TEXTURES, &CEntornVGIView::OnIluminacioTextures)
	ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_TEXTURES, &CEntornVGIView::OnUpdateIluminacioTextures)
	ON_COMMAND(ID_ILUMINACIO_TEXTURA_FITXERIMATGE, &CEntornVGIView::OnIluminacioTexturaFitxerimatge)
	ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_TEXTURA_FITXERIMATGE, &CEntornVGIView::OnUpdateIluminacioTexturaFitxerimatge)
	ON_COMMAND(ID_LLUMS_LLUMAMBIENT, &CEntornVGIView::OnLlumsLlumambient)
	ON_UPDATE_COMMAND_UI(ID_LLUMS_LLUMAMBIENT, &CEntornVGIView::OnUpdateLlumsLlumambient)
	ON_COMMAND(ID_LLUMS_LLUM0, &CEntornVGIView::OnLlumsLlum0)
	ON_UPDATE_COMMAND_UI(ID_LLUMS_LLUM0, &CEntornVGIView::OnUpdateLlumsLlum0)
	ON_COMMAND(ID_SHADERS_SENSE, &CEntornVGIView::OnShadersSense)
	ON_UPDATE_COMMAND_UI(ID_SHADERS_SENSE, &CEntornVGIView::OnUpdateShadersSense)
	ON_COMMAND(ID_SHADERS_GOURAUD, &CEntornVGIView::OnShadersGouraud)
	ON_UPDATE_COMMAND_UI(ID_SHADERS_GOURAUD, &CEntornVGIView::OnUpdateShadersGouraud)
	ON_COMMAND(ID_SHADERS_PHONG, &CEntornVGIView::OnShadersPhong)
	ON_UPDATE_COMMAND_UI(ID_SHADERS_PHONG, &CEntornVGIView::OnUpdateShadersPhong)
	ON_COMMAND(ID_VISTA_SATELIT, &CEntornVGIView::OnVistaSatelit)
	ON_UPDATE_COMMAND_UI(ID_VISTA_SATELIT, &CEntornVGIView::OnUpdateVistaSatelit)
	ON_COMMAND(ID_VISTA_FULLSCREEN, &CEntornVGIView::OnVistaFullscreen)
	ON_UPDATE_COMMAND_UI(ID_VISTA_FULLSCREEN, &CEntornVGIView::OnUpdateVistaFullscreen)
	ON_COMMAND(ID_OBJECTE_TIE, &CEntornVGIView::OnObjecteTie)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_TIE, &CEntornVGIView::OnUpdateObjecteTie)
	ON_COMMAND(ID_ILUMINACIO_PUNTS, &CEntornVGIView::OnIluminacioPunts)
	ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_PUNTS, &CEntornVGIView::OnUpdateIluminacioPunts)
	ON_COMMAND(ID_SHADER_LOAD_FILES, &CEntornVGIView::OnShaderLoadFiles)
	ON_UPDATE_COMMAND_UI(ID_SHADER_LOAD_FILES, &CEntornVGIView::OnUpdateShaderLoadFiles)
	ON_COMMAND(ID_OCULTACIONS_FRONTFACES, &CEntornVGIView::OnOcultacionsFrontFaces)
	ON_UPDATE_COMMAND_UI(ID_OCULTACIONS_FRONTFACES, &CEntornVGIView::OnUpdateOcultacionsFrontFaces)
	ON_COMMAND(ID_LLUMS_LLUM1, &CEntornVGIView::OnLlumsLlum1)
	ON_UPDATE_COMMAND_UI(ID_LLUMS_LLUM1, &CEntornVGIView::OnUpdateLlumsLlum1)
	ON_COMMAND(ID_LLUMS_LLUM2, &CEntornVGIView::OnLlumsLlum2)
	ON_UPDATE_COMMAND_UI(ID_LLUMS_LLUM2, &CEntornVGIView::OnUpdateLlumsLlum2)
	ON_COMMAND(ID_LLUMS_LLUM3, &CEntornVGIView::OnLlumsLlum3)
	ON_UPDATE_COMMAND_UI(ID_LLUMS_LLUM3, &CEntornVGIView::OnUpdateLlumsLlum3)
	ON_COMMAND(ID_LLUMS_LLUM4, &CEntornVGIView::OnLlumsLlum4)
	ON_UPDATE_COMMAND_UI(ID_LLUMS_LLUM4, &CEntornVGIView::OnUpdateLlumsLlum4)
	ON_COMMAND(ID_OBJETE_CORBA_BEZIER, &CEntornVGIView::OnObjeteCorbaBezier)
	ON_UPDATE_COMMAND_UI(ID_OBJETE_CORBA_BEZIER, &CEntornVGIView::OnUpdateObjeteCorbaBezier)
	ON_COMMAND(ID_OBJECTE_CORBA_LEMNISCATA, &CEntornVGIView::OnObjecteCorbaLemniscata)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_CORBA_LEMNISCATA, &CEntornVGIView::OnUpdateObjecteCorbaLemniscata)
	ON_COMMAND(ID_OBJECTE_CORBA_BSPLINE, &CEntornVGIView::OnObjecteCorbaBSpline)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_CORBA_BSPLINE, &CEntornVGIView::OnUpdateObjecteCorbaBSpline)
	ON_COMMAND(ID_OBJECTE_PUNTS_CONTROL, &CEntornVGIView::OnObjectePuntsControl)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_PUNTS_CONTROL, &CEntornVGIView::OnUpdateObjectePuntsControl)
	ON_COMMAND(ID_VISTA_GRIDXY, &CEntornVGIView::OnVistaGridXY)
	ON_UPDATE_COMMAND_UI(ID_VISTA_GRIDXY, &CEntornVGIView::OnUpdateVistaGridXY)
	ON_COMMAND(ID_VISTA_GRIDXZ, &CEntornVGIView::OnVistaGridXZ)
	ON_UPDATE_COMMAND_UI(ID_VISTA_GRIDXZ, &CEntornVGIView::OnUpdateVistaGridXZ)
	ON_COMMAND(ID_VISTA_GRIDYZ, &CEntornVGIView::OnVistaGridYZ)
	ON_UPDATE_COMMAND_UI(ID_VISTA_GRIDYZ, &CEntornVGIView::OnUpdateVistaGridYZ)
	ON_COMMAND(ID_VISTA_GRIDXYZ, &CEntornVGIView::OnVistaGridXYZ)
	ON_UPDATE_COMMAND_UI(ID_VISTA_GRIDXYZ, &CEntornVGIView::OnUpdateVistaGridXYZ)
	ON_COMMAND(ID_ILUMINACIO2SIDES, &CEntornVGIView::OnIluminacio2Sides)
	ON_UPDATE_COMMAND_UI(ID_ILUMINACIO2SIDES, &CEntornVGIView::OnUpdateIluminacio2Sides)
	ON_COMMAND(ID_OBJECTE_MATRIU_PRIMITIVES, &CEntornVGIView::OnObjecteMatriuPrimitives)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_MATRIU_PRIMITIVES, &CEntornVGIView::OnUpdateObjecteMatriuPrimitives)
	ON_COMMAND(ID_OBJECTE_MATRIU_PRIMITIVES_VBO, &CEntornVGIView::OnObjecteMatriuPrimitivesVBO)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_MATRIU_PRIMITIVES_VBO, &CEntornVGIView::OnUpdateObjecteMatriuPrimitivesVBO)
	ON_COMMAND(ID_VISTA_SKYBOX, &CEntornVGIView::OnVistaSkyBox)
	ON_UPDATE_COMMAND_UI(ID_VISTA_SKYBOX, &CEntornVGIView::OnUpdateVistaSkyBox)
	ON_COMMAND(ID_ILUMINACIO_PHONG, &CEntornVGIView::OnIluminacioPhong)
	ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_PHONG, &CEntornVGIView::OnUpdateIluminacioPhong)
	ON_COMMAND(ID_OBJECTE_ARC, &CEntornVGIView::OnObjecteArc)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_ARC, &CEntornVGIView::OnUpdateObjecteArc)
	ON_COMMAND(ID_CAMERA_ESFERICA, &CEntornVGIView::OnCameraEsferica)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_ESFERICA, &CEntornVGIView::OnUpdateCameraEsferica)
	ON_COMMAND(ID_VISTA_ZOOM_ORTO, &CEntornVGIView::OnVistaZoomOrto)
	ON_UPDATE_COMMAND_UI(ID_VISTA_ZOOM_ORTO, &CEntornVGIView::OnUpdateVistaZoomOrto)
	ON_COMMAND(ID_CAMERA_GEODE, &CEntornVGIView::OnCameraGeode)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_GEODE, &CEntornVGIView::OnUpdateCameraGeode)
	ON_COMMAND(ID_CAMERA_ORIGEN_GEODE, &CEntornVGIView::OnCameraOrigenGeode)
	ON_COMMAND(ID_CORBES_TRIEDREFRENET, &CEntornVGIView::OnCorbesTriedreFrenet)
	ON_UPDATE_COMMAND_UI(ID_CORBES_TRIEDREFRENET, &CEntornVGIView::OnUpdateCorbesTriedreFrenet)
	ON_COMMAND(ID_OBJECTE_CUB_RGB, &CEntornVGIView::OnObjecteCubRGB)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_CUB_RGB, &CEntornVGIView::OnUpdateObjecteCubRGB)
	ON_COMMAND(ID_ILUMINACIO_TEXTURA_FLAGINVERTY, &CEntornVGIView::OnIluminacioTexturaFlagInvertY)
	ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_TEXTURA_FLAGINVERTY, &CEntornVGIView::OnUpdateIluminacioTexturaFlagInvertY)
	ON_COMMAND(ID_MATERIAL_REFLMATERIAL, &CEntornVGIView::OnMaterialReflmaterial)
	ON_UPDATE_COMMAND_UI(ID_MATERIAL_REFLMATERIAL, &CEntornVGIView::OnUpdateMaterialReflMaterial)
	ON_COMMAND(ID_OBJECTE_TETRIS, &CEntornVGIView::OnObjecteTetris)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Construcci�n o destrucci�n de CEntornVGIView

CEntornVGIView::CEntornVGIView()
{
	// TODO: agregar aqu� el c�digo de construcci�n
	//	int i = 0;

	//------ Entorn VGI: Inicialitzaci� de les variables globals de CEntornVGIView
	int i;

	// Entorn VGI: Variables de control per Men� C�mera: Esf�rica, Navega, M�bil, Zoom, Satelit, Polars... 
	camera = CAM_ESFERICA;
	mobil = true;	zzoom = true;		zzoomO = false;		satelit = false;

	// Entorn VGI: Variables de control de l'opci� C�mera->Navega?
	n[0] = 0.0;		n[1] = 0.0;		n[2] = 0.0;
	opvN.x = 10.0;	opvN.y = 0.0;		opvN.z = 0.0;
	angleZ = 0.0;
	ViewMatrix = glm::mat4(1.0);		// Inicialitzar a identitat

// Entorn VGI: Variables de control de l'opci� C�mera->Geode?
	OPV_G.R = 15.0;		OPV_G.alfa = 0.0;	OPV_G.beta = 0.0;	// Origen PV en esf�riques per a Vista_Geode

// Entorn VGI: Variables de control per Men� Vista: Pantalla Completa, Pan, dibuixar eixos i grids
	fullscreen = false;
	pan = false;
	eixos = true;	eixos_programID = 0;  eixos_Id = 0;
	sw_grid = false;
	grid.x = false;	grid.y = false;		grid.z = false;		grid.w = false;
	hgrid.x = 0.0;	hgrid.y = 0.0;		hgrid.z = 0.0;		hgrid.w = 0.0;

	// Entorn VGI: Variables opci� Vista->Pan
	fact_pan = 1;
	tr_cpv.x = 0;	tr_cpv.y = 0;	tr_cpv.z = 0;		tr_cpvF.x = 0;	tr_cpvF.y = 0;	tr_cpvF.z = 0;

	// Entorn VGI: Variables de control per les opcions de men� Projecci�, Objecte
	projeccio = CAP;	// projeccio = PERSPECT;
	ProjectionMatrix = glm::mat4(1.0);	// Inicialitzar a identitat
	objecte = CAP;		// objecte = TETERA;

// Entorn VGI: Variables de control Skybox Cube
	SkyBoxCube = false;		skC_programID = 0;
	skC_VAOID.vaoId = -1;	skC_VAOID.vboId = -1;	skC_VAOID.nVertexs = 0;
	cubemapTexture = 0;

	// Entorn VGI: Variables de control del men� Transforma
	transf = false;		trasl = false;		rota = false;		escal = false;
	fact_Tras = 1;		fact_Rota = 90;
	TG.VTras.x = 0.0;	TG.VTras.y = 0.0;	TG.VTras.z = 0;	TGF.VTras.x = 0.0;	TGF.VTras.y = 0.0;	TGF.VTras.z = 0;
	TG.VRota.x = 0;		TG.VRota.y = 0;		TG.VRota.z = 0;	TGF.VRota.x = 0;	TGF.VRota.y = 0;	TGF.VRota.z = 0;
	TG.VScal.x = 1;		TG.VScal.y = 1;		TG.VScal.z = 1;	TGF.VScal.x = 1;	TGF.VScal.y = 1;	TGF.VScal.z = 1;

	transX = false;	transY = false;	transZ = false;
	GTMatrix = glm::mat4(1.0);		// Inicialitzar a identitat

// Entorn VGI: Variables de control per les opcions de men� Ocultacions
	front_faces = true;	test_vis = false;	oculta = false;		back_line = false;

	// Entorn VGI: Variables de control del men� Iluminaci�		
	ilumina = FILFERROS;			ifixe = false;					ilum2sides = false;
	// Reflexions actives: Ambient [1], Difusa [2] i Especular [3]. No actives: Emission [0]. 
	sw_material[0] = false;			sw_material[1] = true;			sw_material[2] = true;			sw_material[3] = true;	sw_material[4] = false;
	sw_material_old[0] = false;		sw_material_old[1] = true;		sw_material_old[2] = true;		sw_material_old[3] = true;	sw_material_old[4] = true;
	textura = false;				t_textura = CAP;				textura_map = true;
	for (i = 0; i < NUM_MAX_TEXTURES; i++) texturesID[i] = -1;
	tFlag_invert_Y = false;

	// Entorn VGI: Variables de control del men� Llums
	// Entorn VGI: Inicialitzaci� variables Llums
	llum_ambient = true;
	for (i = 1; i < NUM_MAX_LLUMS; i++) llumGL[i].encesa = false;
	for (i = 0; i < NUM_MAX_LLUMS; i++) {
		llumGL[i].encesa = false;
		llumGL[i].difusa[0] = 1.0f;	llumGL[i].difusa[1] = 1.0f;	llumGL[i].difusa[2] = 1.0f;	llumGL[i].difusa[3] = 1.0f;
		llumGL[i].especular[0] = 1.0f; llumGL[i].especular[1] = 1.0f; llumGL[i].especular[2] = 1.0f; llumGL[i].especular[3] = 1.0f;
	}

	// LLum 0: �tenuaci� constant (c=1), sobre l'eix Z, no restringida.
	llumGL[0].encesa = true;
	llumGL[0].difusa[0] = 1.0f;			llumGL[0].difusa[1] = 1.0f;			llumGL[0].difusa[2] = 1.0f;		llumGL[0].difusa[3] = 1.0f;
	llumGL[0].especular[0] = 1.0f;		llumGL[0].especular[1] = 1.0f;		llumGL[0].especular[2] = 1.0f;	llumGL[0].especular[3] = 1.0f;

	llumGL[0].posicio.R = 200.0;		llumGL[0].posicio.alfa = 90.0;		llumGL[0].posicio.beta = 0.0;		// Posici� llum (x,y,z)=(0,0,200)
	llumGL[0].atenuacio.a = 0.0;		llumGL[0].atenuacio.b = 0.0;		llumGL[0].atenuacio.c = 1.0;		// Llum sense atenuaci� per dist�ncia (a,b,c)=(0,0,1)
	llumGL[0].restringida = false;
	llumGL[0].spotdirection[0] = 0.0;	llumGL[0].spotdirection[1] = 0.0;	llumGL[0].spotdirection[2] = 0.0;
	llumGL[0].cutoff = 0.0;				llumGL[0].exponent = 0.0;

	// LLum 1: Atenuaci� constant (c=1), sobre l'eix X, no restringida.
	llumGL[1].encesa = false;
	llumGL[1].difusa[0] = 1.0f;			llumGL[1].difusa[1] = 1.0f;			llumGL[1].difusa[2] = 1.0f;		llumGL[1].difusa[3] = 1.0f;
	llumGL[1].especular[0] = 1.0f;		llumGL[1].especular[1] = 1.0f;		llumGL[1].especular[2] = 1.0f;	llumGL[1].especular[3] = 1;

	llumGL[1].posicio.R = 75.0;			llumGL[1].posicio.alfa = 0.0;		llumGL[1].posicio.beta = 0.0;// (x,y,z)=(75,0,0)
	llumGL[1].atenuacio.a = 0.0;		llumGL[1].atenuacio.b = 0.0;		llumGL[1].atenuacio.c = 1.0;
	llumGL[1].restringida = false;
	llumGL[1].spotdirection[0] = 0.0;	llumGL[1].spotdirection[1] = 0.0;	llumGL[1].spotdirection[2] = 0.0;
	llumGL[1].cutoff = 0.0;				llumGL[1].exponent = 0.0;

	// LLum 2: Atenuaci� constant (c=1), sobre l'eix Y, no restringida.
	llumGL[2].encesa = false;
	llumGL[2].difusa[1] = 1.0f;			llumGL[2].difusa[1] = 1.0f;			llumGL[2].difusa[2] = 1.0f;		llumGL[2].difusa[3] = 1.0f;
	llumGL[2].especular[1] = 1.0f;		llumGL[2].especular[1] = 1.0f;		llumGL[2].especular[2] = 1.0f;	llumGL[2].especular[3] = 1;

	llumGL[2].posicio.R = 75.0;			llumGL[2].posicio.alfa = 0.0;		llumGL[2].posicio.beta = 90.0;	// (x,y,z)=(0,75,0)
	llumGL[2].atenuacio.a = 0.0;		llumGL[2].atenuacio.b = 0.0;		llumGL[2].atenuacio.c = 1.0;
	llumGL[2].restringida = false;
	llumGL[2].spotdirection[0] = 0.0;	llumGL[2].spotdirection[1] = 0.0;	llumGL[2].spotdirection[2] = 0.0;
	llumGL[2].cutoff = 0.0;				llumGL[2].exponent = 0.0;

	// LLum 3: Atenuaci� constant (c=1), sobre l'eix Y=X, no restringida.
	llumGL[3].encesa = false;
	llumGL[3].difusa[0] = 1.0f;			llumGL[2].difusa[1] = 1.0f;			llumGL[3].difusa[2] = 1.0f;		llumGL[3].difusa[3] = 1.0f;
	llumGL[3].especular[0] = 1.0f;		llumGL[2].especular[1] = 1.0f;		llumGL[3].especular[2] = 1.0f;	llumGL[3].especular[3] = 1;

	llumGL[3].posicio.R = 75.0;			llumGL[3].posicio.alfa = 45.0;		llumGL[3].posicio.beta = 45.0;// (x,y,z)=(75,75,75)
	llumGL[3].atenuacio.a = 0.0;		llumGL[3].atenuacio.b = 0.0;		llumGL[3].atenuacio.c = 1.0;
	llumGL[3].restringida = false;
	llumGL[3].spotdirection[0] = 0.0;	llumGL[3].spotdirection[1] = 0.0;	llumGL[3].spotdirection[2] = 0.0;
	llumGL[3].cutoff = 0.0;				llumGL[3].exponent = 0.0;

	// LLum 4: Atenuaci� constant (c=1), sobre l'eix -Z, no restringida.
	llumGL[4].encesa = false;
	llumGL[4].difusa[0] = 1.0f;			llumGL[4].difusa[1] = 1.0f;			llumGL[4].difusa[2] = 1.0f;		llumGL[4].difusa[3] = 1.0f;
	llumGL[4].especular[0] = 1.0f;		llumGL[4].especular[1] = 1.0f;		llumGL[4].especular[2] = 1.0f;	llumGL[4].especular[3] = 1;

	llumGL[4].posicio.R = 75.0;			llumGL[4].posicio.alfa = -90.0;		llumGL[4].posicio.beta = 0.0;// (x,y,z)=(0,0,-75)
	llumGL[4].atenuacio.a = 0.0;		llumGL[4].atenuacio.b = 0.0;		llumGL[4].atenuacio.c = 1.0;
	llumGL[4].restringida = false;
	llumGL[4].spotdirection[0] = 0.0;	llumGL[4].spotdirection[1] = 0.0;	llumGL[4].spotdirection[2] = 0.0;
	llumGL[4].cutoff = 0.0;				llumGL[4].exponent = 0.0;

	// LLum #5:
	llumGL[5].encesa = false;
	llumGL[5].difusa[0] = 1.0f;			llumGL[5].difusa[1] = 1.0f;			llumGL[5].difusa[2] = 1.0f;		llumGL[5].difusa[3] = 1.0f;
	llumGL[5].especular[0] = 1.0f;		llumGL[5].especular[1] = 1.0f;		llumGL[5].especular[2] = 1.0f;	llumGL[5].especular[3] = 1;

	llumGL[5].posicio.R = 0.0;			llumGL[5].posicio.alfa = 0.0;		llumGL[5].posicio.beta = 0.0; // Cap posici� definida
	llumGL[5].atenuacio.a = 0.0;		llumGL[5].atenuacio.b = 0.0;		llumGL[5].atenuacio.c = 1.0;
	llumGL[5].restringida = false;
	llumGL[5].spotdirection[0] = 0.0;	llumGL[5].spotdirection[1] = 0.0;	llumGL[5].spotdirection[2] = 0.0;
	llumGL[5].cutoff = 0.0;				llumGL[5].exponent = 0.0;

	// LLum #6: Llum Vaixell, configurada a la funci� vaixell() en escena.cpp.
	llumGL[6].encesa = false;
	llumGL[6].difusa[0] = 1.0f;			llumGL[6].difusa[1] = 1.0f;			llumGL[6].difusa[2] = 1.0f;		llumGL[6].difusa[3] = 1.0f;
	llumGL[6].especular[0] = 1.0f;		llumGL[6].especular[1] = 1.0f;		llumGL[6].especular[2] = 1.0f;	llumGL[6].especular[3] = 1;

	llumGL[6].posicio.R = 0.0;			llumGL[6].posicio.alfa = 0.0;		llumGL[6].posicio.beta = 0.0; // Cap posici� definida, definida en funci� vaixell() en escena.cpp
	llumGL[6].atenuacio.a = 0.0;		llumGL[6].atenuacio.b = 0.0;		llumGL[6].atenuacio.c = 1.0;
	llumGL[6].restringida = false;
	llumGL[6].spotdirection[0] = 0.0;	llumGL[6].spotdirection[1] = 0.0;	llumGL[6].spotdirection[2] = 0.0;
	llumGL[6].cutoff = 0.0;				llumGL[6].exponent = 0.0;

	// LLum #7: Llum Far, configurada a la funci� faro() en escena.cpp.
	llumGL[7].encesa = false;
	llumGL[7].difusa[0] = 1.0f;		llumGL[7].difusa[1] = 1.0f;			llumGL[7].difusa[2] = 1.0f;		llumGL[7].difusa[3] = 1.0f;
	llumGL[7].especular[0] = 1.0f;		llumGL[7].especular[1] = 1.0f;		llumGL[7].especular[2] = 1.0f;	llumGL[7].especular[3] = 1;

	llumGL[7].posicio.R = 0.0;			llumGL[7].posicio.alfa = 0.0;		llumGL[7].posicio.beta = 0.0; // Cap posici� definida, definida en funci� faro() en escena.cpp
	llumGL[7].atenuacio.a = 0.0;		llumGL[7].atenuacio.b = 0.0;		llumGL[7].atenuacio.c = 1.0;
	llumGL[7].restringida = false;
	llumGL[7].spotdirection[0] = 0.0;	llumGL[7].spotdirection[1] = 0.0;	llumGL[7].spotdirection[2] = 0.0;
	llumGL[7].cutoff = 0.0;				llumGL[7].exponent = 0.0;

	// Entorn VGI: Variables de control del men� Shaders
		//InitAPI();
	sw_shader = false;				shader_menu = CAP;				shader_programID = 0;

	// Entorn VGI: Variables de control dels botons de mouse
	m_PosEAvall = (0, 0);		m_PosDAvall = (0, 0);
	m_ButoEAvall = false;		m_ButoDAvall = false;
	m_EsfeEAvall.R = 0.0;		m_EsfeEAvall.alfa = 0.0;	m_EsfeEAvall.beta = 0.0;
	m_EsfeIncEAvall.R = 0.0;	m_EsfeIncEAvall.alfa = 0.0;	m_EsfeIncEAvall.beta = 0.0;

	// Entorn VGI: Variables que controlen par�metres visualitzaci�: Mides finestra Windows i PV
	w = 0;				h = 0;								// Mides finestra
	w_old = 0;			h_old = 0;							// Copia mides finestre per a FullScreen
	OPV.R = 15.0;		OPV.alfa = 0.0;		OPV.beta = 0.0;	// Origen PV en esf�riques
	Vis_Polar = POLARY;

	// Entorn VGI: Color de fons i de l'objecte
	fonsR = true;		fonsG = true;		fonsB = true;
	c_fons.r = 0.0;		c_fons.g = 0.0;		c_fons.b = 0.0;
	sw_color = false;
	col_obj.r = 1.0;	col_obj.g = 1.0;	col_obj.b = 1.0;		col_obj.a = 1.0;

	// Entorn VGI: Objecte 3DS
	Ob3DS = NULL;		vaoId_3DS = 0;		nvert_3DS = 0;

	// Entorn VGI: Objecte OBJ
	ObOBJ = NULL;		vaoId_OBJ = 0;		nvert_OBJ = 0;

	// VGI: OBJECTE --> Corba B-Spline i Bezier
	npts_T = 0;
	for (i = 0; i < MAX_PATCH_CORBA; i = i++)
	{
		PC_t[i].x = 0.0;
		PC_t[i].y = 0.0;
		PC_t[i].z = 0.0;
	}

	pas_CS = PAS_BSPLINE;
	sw_Punts_Control = false;

	// TRIEDRE DE FRENET / DARBOUX: VT: vector Tangent, VNP: Vector Normal Principal, VBN: vector BiNormal
	dibuixa_TriedreFrenet = false;
	VT = { 0.0, 0.0, 1.0 };		VNP = { 1.0, 0.0, 0.0 };	VBN = { 0.0, 1.0, 0.0 };

	// Entorn VGI: Variables del Timer
	t = 0;			anima = false;

	// Entorn VGI: Variables de l'objecte FRACTAL
	t_fractal = CAP;	soroll = 'C';
	pas = 64;			pas_ini = 64;
	sw_il = true;		palcolFractal = false;

	// Entorn VGI: Altres variables
	mida = 1.0;			nom = "";		buffer = "";

	// Entorn VGI: Inicialitzaci� de les llibreries DevIL per a la c�rrega de textures i fitxers .3DS
	ilInit();					// Inicialitzar llibreria IL
	iluInit();					// Inicialitzar llibreria ILU
	ilutRenderer(ILUT_OPENGL);	// Inicialitzar llibreria ILUT per a OpenGL

/* Entorn VGI : Activaci� el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

// C�rrega SHADERS
// C�rrega Shader Eixos
		//fprintf(stderr, "Eixos: \n");
	if (!eixos_programID) eixos_programID = shaderEixos.loadFileShaders(".\\shaders\\eixos.VERT", ".\\shaders\\eixos.FRAG");

// C�rrega Shader de Gouraud
	sw_shader = true;				shader_menu = GOURAUD_SHADER;
	if (!shader_programID) shader_programID = shaderLighting.loadFileShaders(".\\shaders\\gouraud_shdrML.vert", ".\\shaders\\gouraud_shdrML.frag");

// Entorn VGI: Creaci� de la llista que dibuixar� els eixos Coordenades M�n. Funci� on est� codi per dibuixar eixos
	if (!eixos_Id) eixos_Id = deixos();						// Funci� que defineix els Eixos Coordenades M�n com un VAO.

// Entorn VGI: Activaci� el contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
*/

// Entorn VGI: Definir desplegable per a Full Screen
	ContextMenu = new CMenu();
	if (!ContextMenu->LoadMenu(IDR_MENU_WINDOW))	AfxMessageBox(_T("Fail to create context menu"));

}

CEntornVGIView::~CEntornVGIView()
{
}

BOOL CEntornVGIView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: modificar aqu� la clase Window o los estilos cambiando
	//  CREATESTRUCT cs

	// An OpenGL window must be created with the following flags and must not
	// include CS_PARENTDC for the class style.
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// Impresi�n de CEntornVGIView

void CEntornVGIView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CEntornVGIView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Preparaci�n predeterminada
	return DoPreparePrinting(pInfo);
}

void CEntornVGIView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: agregar inicializaci�n adicional antes de imprimir
}

void CEntornVGIView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: agregar limpieza despu�s de imprimir
}

void CEntornVGIView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Diagn�sticos de CEntornVGIView

#ifdef _DEBUG
void CEntornVGIView::AssertValid() const
{
	CView::AssertValid();
}

void CEntornVGIView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEntornVGIDoc* CEntornVGIView::GetDocument() const // La versi�n de no depuraci�n est� alineada
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEntornVGIDoc)));
	return (CEntornVGIDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// Controladores de mensaje de CEntornVGIView

int CEntornVGIView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Agregue aqu� su c�digo de creaci�n especializado

		//CDC* pDC = GetDC();
		//m_glRenderer.CreateGLContext(pDC);
	m_pDC = new CClientDC(this);

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.iLayerType = PFD_MAIN_PLANE;

	// Entorn VGI: Format del pixel que millor s'ajusti al descrit en pfd
		//int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);
	int nPixelFormat = ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd);

	if (!nPixelFormat)
	{
		::MessageBoxW(NULL, L"Error en SetPixelFormat", L"Error", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);		// This sends a message telling the program to quit
		return false;
	}

	// Activaci� format pixel per al contexte dispositiu
		//BOOL bResult = SetPixelFormat(pDC->m_hDC, nPixelFormat, &pfd);
	BOOL bResult = SetPixelFormat(m_pDC->GetSafeHdc(), nPixelFormat, &pfd);

	if (!bResult)
	{
		::MessageBoxW(NULL, L"Error en SetPixelFormat", L"Error", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);		// This sends a message telling the program to quit
		return false;
	}

	// Entorn VGI: Creaci� contexte generaci� OpenGL
		// m_hRC=wglCreateContext(m_hDC);
	m_hRC = wglCreateContext(m_pDC->GetSafeHdc());
	if (!m_hRC)
	{
		::MessageBoxW(NULL, L"Error en GL Rendering Context", L"Error", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);		// This sends a message telling the program to quit
		return false;
	}

	// OPENGL 2.0 - OPENGL 3.0 **********************************************************
		// --- OpenGL 3.x ---
	HGLRC tempContext = wglCreateContext(m_pDC->GetSafeHdc());
	wglMakeCurrent(m_pDC->GetSafeHdc(), tempContext);

	//Get access to modern OpenGL functionality from this old style context.
	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit())
	{
		AfxMessageBox(_T("GLEW could not be initialized!"));
		return FALSE;
	}

	//InitAPI();

	//	if (glewIsExtensionSupported("GLEW_ARB_vertex_shader"))
	//	AfxMessageBox(_T("INFO: GLEW_ARB_vertex_shader supported, proceeding\n"));
	//	else AfxMessageBox(_T("INFO: GLEW_ARB_vertex_shader NOT supported, proceeding\n"));
	//
	//	if (glewIsExtensionSupported("GLEW_ARB_fragment_shader"))
	//	AfxMessageBox(_T("INFO: GLEW_ARB_fragment_shader supported, proceeding\n"));
	//	else AfxMessageBox(_T("INFO: GLEW_ARB_fragment_shader NOT supported, proceeding\n"));
	//
	if (glewIsSupported("GL_VERSION_2_0")) //(GLEW_VERSION_2_0)
	{
	} //AfxMessageBox(_T("INFO: OpenGL 2.0 supported!. Proceed\n"));
	else
	{
		AfxMessageBox(_T("INFO: OpenGL 2.0 not supported!. Exit\n"));
		//return EXIT_FAILURE;
	}


	int major, minor;
	GetGLVersion(&major, &minor);

	if (major < 3 || (major == 3 && minor < 2))
		AfxMessageBox(_T("OpenGL 3.2 is not supported!. Exit\n"));
	//	else 
	//		AfxMessageBox(_T("OpenGL 3.2 is supported!. Proceed"));

	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB, //0x9126 , 0x00000001,
		0
	};

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	if (wglCreateContextAttribsARB != NULL)
	{
		m_hrc = wglCreateContextAttribsARB(m_pDC->GetSafeHdc(), 0, attribs);
	}

	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hrc);

	// Entorn VGI: Inicialitzaci� de funcions de shaders per a OpenGL 2.0
	InitAPI();

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(tempContext);

	if (!m_hrc)
	{
		AfxMessageBox(_T("OpenGL 3.3+ was not created!"));
		return false;
	}

	// ------------ - Entorn VGI : Enable OpenGL debug context if context allows for DEBUG CONTEXT (GL4.3)
	if (GLEW_VERSION_4_3)
	{
		GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
			glDebugMessageCallback((GLDEBUGPROC)wglGetProcAddress("glDebugOutput"), nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}
	}

	// Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	return true;
}

void CEntornVGIView::InitAPI()
{
	/*	Vendor, Renderer, Version, Shading Laguage Version i Extensions suportades per la placa gr�fica gravades en fitxer extensions.txt
		CString nomf;
		nomf = "extensions.txt";
		char *nomfitxer = CString2Char(nomf);
		char* str = (char*)glGetString(GL_VENDOR);
		FILE* f = fopen(nomfitxer, "w");
		if(f)	{	fprintf(f,"VENDOR: %s\n",str);
					str = (char*)glGetString(GL_RENDERER);
					fprintf(f, "RENDERER: %s\n", str);
					str = (char*)glGetString(GL_VERSION);
					fprintf(f, "VERSION: %s\n", str);
					str = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
					fprintf(f, "SHADING_LANGUAGE_VERSION: %s\n", str);
					str = (char*)glGetString(GL_EXTENSIONS);
					fprintf(f, "EXTENSIONS: %s\n", str);
					fclose(f);
				}
		*/

		// Program
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
	glUniform1iv = (PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1iv");
	glUniform2iv = (PFNGLUNIFORM2IVPROC)wglGetProcAddress("glUniform2iv");
	glUniform3iv = (PFNGLUNIFORM3IVPROC)wglGetProcAddress("glUniform3iv");
	glUniform4iv = (PFNGLUNIFORM4IVPROC)wglGetProcAddress("glUniform4iv");
	glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
	glUniform1fv = (PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fv");
	glUniform2fv = (PFNGLUNIFORM2FVPROC)wglGetProcAddress("glUniform2fv");
	glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
	glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
	glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)wglGetProcAddress("glVertexAttrib1f");
	glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)wglGetProcAddress("glVertexAttrib1fv");
	glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)wglGetProcAddress("glVertexAttrib2fv");
	glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)wglGetProcAddress("glVertexAttrib3fv");
	glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)wglGetProcAddress("glVertexAttrib4fv");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
	glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)wglGetProcAddress("glGetActiveUniform");

	// Shader
	glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");

	// VBO
	glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	glBufferSubData = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData");
	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
}

void CEntornVGIView::GetGLVersion(int* major, int* minor)
{
	// for all versions
	char* ver = (char*)glGetString(GL_VERSION); // ver = "3.2.0"

	*major = ver[0] - '0';
	if (*major >= 3)
	{
		// for GL 3.x
		glGetIntegerv(GL_MAJOR_VERSION, major);		// major = 3
		glGetIntegerv(GL_MINOR_VERSION, minor);		// minor = 2
	}
	else
	{
		*minor = ver[2] - '0';
	}

	// GLSL
	ver = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);	// 1.50 NVIDIA via Cg compiler
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
	const GLchar* message, const void* userParam)
{
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return; // ignore these non-significant error codes

	fprintf(stderr, "---------------\n");
	fprintf(stderr, "Debug message ( %3i %s \n", id, message);

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             fprintf(stderr, "Source: API \n"); break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   fprintf(stderr, "Source: Window System \n"); break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: fprintf(stderr, "Source: Shader Compiler \n"); break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     fprintf(stderr, "Source: Third Party \n"); break;
	case GL_DEBUG_SOURCE_APPLICATION:     fprintf(stderr, "Source: Application \n"); break;
	case GL_DEBUG_SOURCE_OTHER:           fprintf(stderr, "Source: Other \n"); break;
	} //std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               fprintf(stderr, "Type: Error\n"); break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: fprintf(stderr, "Type: Deprecated Behaviour\n"); break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  fprintf(stderr, "Type: Undefined Behaviour\n"); break;
	case GL_DEBUG_TYPE_PORTABILITY:         fprintf(stderr, "Type: Portability\n"); break;
	case GL_DEBUG_TYPE_PERFORMANCE:         fprintf(stderr, "Type: Performance\n"); break;
	case GL_DEBUG_TYPE_MARKER:              fprintf(stderr, "Type: Marker\n"); break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          fprintf(stderr, "Type: Push Group\n"); break;
	case GL_DEBUG_TYPE_POP_GROUP:           fprintf(stderr, "Type: Pop Group\n"); break;
	case GL_DEBUG_TYPE_OTHER:               fprintf(stderr, "Type: Other\n"); break;
	} //std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         fprintf(stderr, "Severity: high\n"); break;
	case GL_DEBUG_SEVERITY_MEDIUM:       fprintf(stderr, "Severity: medium\n"); break;
	case GL_DEBUG_SEVERITY_LOW:          fprintf(stderr, "Severity: low\n"); break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: fprintf(stderr, "Severity: notification\n"); break;
	} //std::cout << std::endl;
	//std::cout << std::endl;
	fprintf(stderr, "\n");
}


void CEntornVGIView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: Agregue aqu� su c�digo de controlador de mensajes
	CDC* pDC = GetDC();
	ReleaseDC(pDC);
}


void CEntornVGIView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Agregue aqu� su c�digo de controlador de mensajes

	// A resize event occured; cx and cy are the window's new width and height.
	// Find the OpenGL change size function given in the Lab 1 notes and call it here

	// Entorn VGI: MODIFICACI� ->Establim les mides de la finestra actual
	w = cx;
	h = cy;

}


void CEntornVGIView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	CDC* pDC = GetDC();
	//m_glRenderer.PrepareScene(pDC);
	ReleaseDC(pDC);
}


/////////////////////////////////////////////////////////////////////////////
// Dibujo de CEntornVGIView

void CEntornVGIView::OnDraw(CDC* /*pDC*/)
{

	// TODO: agregar aqu� el c�digo de dibujo para datos nativos

	CEntornVGIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
}


void CEntornVGIView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
// TODO: Agregue aqu� su c�digo de controlador de mensajes
	GLdouble vpv[3] = { 0.0, 0.0, 1.0 };

	// Entorn VGI: Activaci� el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	// Cridem a les funcions de l'escena i la projecci� segons s'hagi 
	// seleccionat una projecci� o un altra
	switch (projeccio)
	{
	case AXONOM:
		// PROJECCI� AXONOM�TRICA
		// Activaci� del retall de pantalla
		glEnable(GL_SCISSOR_TEST);

		// Retall
		glScissor(0, 0, w, h);
		glViewport(0, 0, w, h);

		// Aqu� farem les crides per a definir Viewport, Projecci� i C�mara: INICI -------------------------

		// Aqu� farem les cridesper a definir Viewport, Projecci� i C�mara:: FI -------------------------
				// Dibuixar Model (escena)
		configura_Escena();     // Aplicar Transformacions Geometriques segons persiana Transformacio i configurar objectes
		dibuixa_Escena2();		// Dibuix geometria de l'escena amb comandes GL.

// Intercanvia l'escena al front de la pantalla
		SwapBuffers(m_pDC->GetSafeHdc());
		break;

	case ORTO:
		// PROJECCI� ORTOGR�FICA
		// Activaci� del retall de pantalla
		glEnable(GL_SCISSOR_TEST);

		// Retall
		glScissor(0, 0, w, h);
		glViewport(0, 0, w, h);

		// Fons condicionat al color de fons
		if ((c_fons.r < 0.5) || (c_fons.g < 0.5) || (c_fons.b < 0.5))
			FonsB();
		else
			FonsN();

		// Aqu� farem les quatre crides a ProjeccioOrto i Ortografica per obtenir 
		// les quatre vistes ortogr�fiques
		// ---------- Entorn VGI: DESCOMENTAR QUAN S'IMPLEMENTI PROJECCIO ORTOGR�FICA
		// PLANTA (Inferior Esquerra)
				// Definici� de Viewport, Projecci� i C�mara
		ProjectionMatrix = Projeccio_Orto();
		ViewMatrix = Vista_Ortografica(shader_programID, 0, OPV.R, c_fons, col_obj, objecte, mida, pas, front_faces, oculta,
			test_vis, back_line, ilumina, llum_ambient, llumGL, ifixe, ilum2sides,
			eixos, grid, hgrid);
		// Dibuix de l'Objecte o l'Escena
		configura_Escena();     // Aplicar Transformacions Geometriques segons persiana Transformacio i configurar objectes
		dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.

// ISOM�TRICA (Inferior Dreta)
		// Definici� de Viewport, Projecci� i C�mara
		ProjectionMatrix = Projeccio_Orto();
		ViewMatrix = Vista_Ortografica(shader_programID, 3, OPV.R, c_fons, col_obj, objecte, mida, pas, front_faces, oculta,
			test_vis, back_line, ilumina, llum_ambient, llumGL, ifixe, ilum2sides,
			eixos, grid, hgrid);
		// Dibuix de l'Objecte o l'Escena
		//glPushMatrix();
		configura_Escena();     // Aplicar Transformacions Geometriques segons persiana Transformacio i configurar objectes
		dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.
		//glPopMatrix();

// AL�AT (Superior Esquerra)
		// Definici� de Viewport, Projecci� i C�mara
		ProjectionMatrix = Projeccio_Orto();
		ViewMatrix = Vista_Ortografica(shader_programID, 1, OPV.R, c_fons, col_obj, objecte, mida, pas, front_faces, oculta,
			test_vis, back_line, ilumina, llum_ambient, llumGL, ifixe, ilum2sides,
			eixos, grid, hgrid);
		// Dibuix de l'Objecte o l'Escena
		configura_Escena();     // Aplicar Transformacions Geom?triques segons persiana Transformacio i configurar objectes
		dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.

// PERFIL (Superior Dreta)
		// Definici� de Viewport, Projecci� i C�mara
		ProjectionMatrix = Projeccio_Orto();
		ViewMatrix = Vista_Ortografica(shader_programID, 2, OPV.R, c_fons, col_obj, objecte, mida, pas, front_faces, oculta,
			test_vis, back_line, ilumina, llum_ambient, llumGL, ifixe, ilum2sides,
			eixos, grid, hgrid);
		// Dibuix de l'Objecte o l'Escena
		configura_Escena();     // Aplicar Transformacions Geom?triques segons persiana Transformacio i configurar objectes
		dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.

// Intercanvia l'escena al front de la pantalla
		SwapBuffers(m_pDC->GetSafeHdc());
		break;

	case PERSPECT:
		// PROJECCI� PERSPECTIVA
				//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Set Perspective Calculations To Most Accurate
		glDisable(GL_SCISSOR_TEST);		// Desactivaci� del retall de pantalla

		// Definici� de Viewport, Projecci� i C�mara
		ProjectionMatrix = Projeccio_Perspectiva(shader_programID, 0, 0, w, h, OPV.R);

		// Definici� de la c�mera.
		if (camera == CAM_ESFERICA) {
			n[0] = 0;		n[1] = 0;		n[2] = 0;
			ViewMatrix = Vista_Esferica(shader_programID, OPV, Vis_Polar, pan, tr_cpv, tr_cpvF, c_fons, col_obj, objecte, mida, pas,
				front_faces, oculta, test_vis, back_line,
				ilumina, llum_ambient, llumGL, ifixe, ilum2sides,
				eixos, grid, hgrid, camerax);
		}
		else if (camera == CAM_NAVEGA) {
			ViewMatrix = Vista_Navega(shader_programID, opvN, false, n, vpv, pan, tr_cpv, tr_cpvF, c_fons, col_obj, objecte, true, pas,
				front_faces, oculta, test_vis, back_line,
				ilumina, llum_ambient, llumGL, ifixe, ilum2sides,
				eixos, grid, hgrid);
		}
		else if (camera == CAM_GEODE) {
			ViewMatrix = Vista_Geode(shader_programID, OPV_G, Vis_Polar, pan, tr_cpv, tr_cpvF, c_fons, col_obj, objecte, mida, pas,
				front_faces, oculta, test_vis, back_line,
				ilumina, llum_ambient, llumGL, ifixe, ilum2sides,
				eixos, grid, hgrid);
		}

		// Dibuix de l'Objecte o l'Escena
		configura_Escena();     // Aplicar Transformacions Geometriques segons persiana Transformacio i configurar objectes.
		dibuixa_Escena2();		// Dibuix geometria de l'escena amb comandes GL.

// Intercanvia l'escena al front de la pantalla
		SwapBuffers(m_pDC->GetSafeHdc());
		break;

	default:
		// C�rrega SHADERS
		// C�rrega Shader Eixos
		if (!eixos_programID) eixos_programID = shaderEixos.loadFileShaders(".\\shaders\\eixos.VERT", ".\\shaders\\eixos.FRAG");

		// C�rrega Shader de Gouraud
		sw_shader = true;				shader_menu = GOURAUD_SHADER;
		if (!shader_programID) shader_programID = shaderLighting.loadFileShaders(".\\shaders\\gouraud_shdrML.vert", ".\\shaders\\gouraud_shdrML.frag");

		// Entorn VGI: Creaci� de la llista que dibuixar� els eixos Coordenades M�n. Funci� on est� codi per dibuixar eixos	
		if (!eixos_Id) eixos_Id = deixos();						// Funci� que defineix els Eixos Coordenades M�n com un VAO.

// Definci� projecci� PERSPECTIVA

		projeccio = PERSPECT;
		OnIluminacioGouraud();
		OnVistaSkyBox();
		mainTetris();


		// Crida a la funci� Fons Blanc
		FonsB();

		// Intercanvia l'escena al front de la pantalla
		SwapBuffers(m_pDC->GetSafeHdc());
		break;
	}

	// Entorn VGI: Activaci� el contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	//  Actualitzar la barra d'estat de l'aplicaci� amb els valors R,A,B,PVx,PVy,PVz
	Barra_Estat();
}


// configura_Escena: Funcio que configura els parametres de Model i dibuixa les
//                   primitives OpenGL dins classe Model
void CEntornVGIView::configura_Escena()
{
	// Aplicar Transformacions Geometriques segons persiana Transformacio i Quaternions
	GTMatrix = instancia(transf, TG, TGF);
}

// dibuixa_Escena: Funcio que crida al dibuix dels diferents elements de l'escana
void CEntornVGIView::dibuixa_Escena()
{
	//	Dibuix SkyBox C�bic.
	if (SkyBoxCube) dibuixa_Skybox(skC_programID, cubemapTexture, Vis_Polar, ProjectionMatrix, ViewMatrix);

	//	Dibuix Coordenades M�n i Reixes.
	dibuixa_Eixos(eixos_programID, eixos, eixos_Id, grid, hgrid, ProjectionMatrix, ViewMatrix);

	//	GTMatrix = glm::scalef(GTMatrix,vec3());			// Escalat d'objectes, per adequar-los a les vistes ortogr�fiques (Pr�ctica 2)

	//	Dibuix geometria de l'escena amb comandes GL.

	for (int i = 0; i < 7; i++) {
		dibuixa_EscenaGL(shader_programID, eixos, eixos_Id, grid, hgrid, objecte, col_obj, sw_material,
			textura, texturesID, textura_map, tFlag_invert_Y,
			npts_T, PC_t, pas_CS, sw_Punts_Control, dibuixa_TriedreFrenet,
			FIT_3DS, FIT_OBJ + i, // VAO's i nombre de v�rtexs dels objectes 3DS i OBJ
			ViewMatrix, GTMatrix);
	}



	//	Dibuix Coordenades M�n i Reixes.
			//dibuixa_Eixos(eixos_programID, eixos, eixos_Id, grid, hgrid, ProjectionMatrix, ViewMatrix);

	//	Dibuix Coordenades M�n i Reixes.
			//if (SkyBoxCube) dibuixa_Skybox(skC_programID, cubemapTexture, ProjectionMatrix, ViewMatrix);
}

void CEntornVGIView::dibuixa_Escena2()
{
	//	Dibuix SkyBox C�bic.
	if (SkyBoxCube) dibuixa_Skybox(skC_programID, cubemapTexture, Vis_Polar, ProjectionMatrix, ViewMatrix);

	//	Dibuix Coordenades M�n i Reixes.
	dibuixa_Eixos(eixos_programID, eixos, eixos_Id, grid, hgrid, ProjectionMatrix, ViewMatrix);

	//	GTMatrix = glm::scalef(GTMatrix,vec3());			// Escalat d'objectes, per adequar-los a les vistes ortogr�fiques (Pr�ctica 2)

	//	Dibuix geometria de l'escena amb comandes GL.

	objecte = PROPI;


	glm::mat4 posite(1.0);
	glm::mat4 sendposite(1.0);
	int boardI = 0, boardJ = 0, boardK = 0;
	for (int boardI = 0; boardI < m_board.m_blocks.size(); boardI++)
	{
		for (int boardJ = 0; boardJ < m_board.m_blocks[boardI].size(); boardJ++) // TREURE EL -6
		{
			for (int boardK = 0; boardK < m_board.m_blocks[boardI][boardJ].size(); boardK++)
			{
				sendposite = translate(posite, vec3(boardI * 2 + 1, boardJ * 2 + 1, boardK * 2 + 1));
				if (!m_board.m_blocks[boardI][boardJ][boardK].m_lliure)
				{
					dibuixa_EscenaGL(shader_programID, eixos, eixos_Id, grid, hgrid, objecte, col_obj, sw_material,
						textura, texturesID, textura_map, tFlag_invert_Y,
						npts_T, PC_t, pas_CS, sw_Punts_Control, dibuixa_TriedreFrenet,
						FIT_3DS, m_board.m_blocks[boardI][boardJ][boardK].getIdVao(), // VAO's i nombre de v�rtexs dels objectes 3DS i OBJ
						ViewMatrix,
						sendposite);
				}


			}
		}
	}

	/*
	dibuixa_EscenaGL(shader_programID, eixos, eixos_Id, grid, hgrid, objecte, col_obj, sw_material,
		textura, texturesID, textura_map, tFlag_invert_Y,
		npts_T, PC_t, pas_CS, sw_Punts_Control, dibuixa_TriedreFrenet,
		FIT_3DS, pieces[2].getIdVao(), // VAO's i nombre de v�rtexs dels objectes 3DS i OBJ
		ViewMatrix,
		pieces[2].getMatrix());

	*/

	// Dibuix del tauler en escena

	dibuixa_EscenaGL(shader_programID, eixos, eixos_Id, grid, hgrid, objecte, col_obj, sw_material,
		textura, texturesID, textura_map, tFlag_invert_Y,
		npts_T, PC_t, pas_CS, sw_Punts_Control, dibuixa_TriedreFrenet,
		FIT_3DS, m_board.getIdVao(), // VAO's i nombre de v�rtexs dels objectes 3DS i OBJ
		ViewMatrix,
		m_board.getMatrix());
	glm::mat4 rotM = toMat4(pieces[numPiece].getQuaternio());
	dibuixa_EscenaGL(shader_programID, eixos, eixos_Id, grid, hgrid, objecte, col_obj, sw_material,
		textura, texturesID, textura_map, tFlag_invert_Y,
		npts_T, PC_t, pas_CS, sw_Punts_Control, dibuixa_TriedreFrenet,
		FIT_3DS, pieces[numPiece].getIdVao(), // VAO's i nombre de v�rtexs dels objectes 3DS i OBJ
		ViewMatrix, pieces[numPiece].getMatrix() * rotM);



	// Matriu de transformacions de la pe�a en questio



	//	Dibuix Coordenades M�n i Reixes.
			//dibuixa_Eixos(eixos_programID, eixos, eixos_Id, grid, hgrid, ProjectionMatrix, ViewMatrix);

	//	Dibuix Coordenades M�n i Reixes.
			//if (SkyBoxCube) dibuixa_Skybox(skC_programID, cubemapTexture, ProjectionMatrix, ViewMatrix);
}


// Barra_Estat: Actualitza la barra d'estat (Status Bar) de l'aplicaci� amb els
//      valors R,A,B,PVx,PVy,PVz en Visualitzaci� Interactiva.
void CEntornVGIView::Barra_Estat()
{
	CString sss;
	CEsfe3D OPVAux;
	double PVx, PVy, PVz;
	//GLdouble color;

// Status Bar fitxer fractal
	if (nom != "") GetStatusBar().SetPaneText(0, nom);

	// C�lcul dels valors per l'opci� Vista->Navega
	if (projeccio != CAP && projeccio != ORTO) {
		if (camera == CAM_ESFERICA)
		{	// C�mera Esf�rica
			OPVAux.R = OPV.R; OPVAux.alfa = OPV.alfa; OPVAux.beta = OPV.beta;
		}
		else if (camera == CAM_NAVEGA)
		{	// C�mera Navega
			OPVAux.R = sqrt(opvN.x * opvN.x + opvN.y * opvN.y + opvN.z * opvN.z);
			OPVAux.alfa = (asin(opvN.z / OPVAux.R) * 180) / PI;
			OPVAux.beta = (atan(opvN.y / opvN.x)) * 180 / PI;
		}
		else {	// C�mera Geode
			OPVAux.R = OPV_G.R; OPVAux.alfa = OPV_G.alfa; OPVAux.beta = OPV_G.beta;
		}
	}
	else {
		OPVAux.R = OPV.R; OPVAux.alfa = OPV.alfa; OPVAux.beta = OPV.beta;
	}

	// Status Bar R Origen Punt de Vista
	if (projeccio == CAP) buffer = "       ";
	else if (projeccio == ORTO) buffer = " ORTO   ";
	else if (camera == CAM_NAVEGA) buffer = " NAV   ";
	else buffer.Format(_T("%.1f"), OPVAux.R);
	sss = _T("R=") + buffer;
	// Refrescar posici� R Status Bar
	GetStatusBar().SetPaneText(1, sss);

	// Status Bar angle alfa Origen Punt de Vista
	if (projeccio == CAP) buffer = "       ";
	else if (projeccio == ORTO) buffer = "ORTO   ";
	else if (camera == CAM_NAVEGA) buffer = " NAV   ";
	else buffer.Format(_T("%.1f"), OPVAux.alfa);
	sss = _T("a=") + buffer;
	// Refrescar posici� angleh Status Bar
	GetStatusBar().SetPaneText(2, sss);

	// Status Bar angle beta Origen Punt de Vista
	if (projeccio == CAP) buffer = "       ";
	else if (projeccio == ORTO) buffer = "ORTO   ";
	else if (camera == CAM_NAVEGA) buffer = " NAV   ";
	else buffer.Format(_T("%.1f"), OPVAux.beta);
	sss = _T("�=") + buffer;
	// Refrescar posici� anglev Status Bar
	GetStatusBar().SetPaneText(3, sss);

	// Transformaci� PV de Coord. esf�riques (R,anglev,angleh) --> Coord. cartesianes (PVx,PVy,PVz)
	if (camera == CAM_NAVEGA) { PVx = opvN.x; PVy = opvN.y; PVz = opvN.z; }
	else {
		if (Vis_Polar == POLARZ) {
			PVx = OPVAux.R * cos(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
			PVy = OPVAux.R * sin(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
			PVz = OPVAux.R * sin(OPVAux.alfa * PI / 180);
		}
		else if (Vis_Polar == POLARY) {
			PVx = OPVAux.R * sin(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
			PVy = OPVAux.R * sin(OPVAux.alfa * PI / 180);
			PVz = OPVAux.R * cos(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
		}
		else {
			PVx = OPVAux.R * sin(OPVAux.alfa * PI / 180);
			PVy = OPVAux.R * cos(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
			PVz = OPVAux.R * sin(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
		}
	}

	// Status Bar PVx
	if (projeccio == CAP) buffer = "       ";
	else if (pan) double2CString(tr_cpv.x);
	else double2CString(PVx);
	sss = _T("PVx=") + buffer;
	// Refrescar posici� PVx Status Bar
	GetStatusBar().SetPaneText(4, sss);

	// Status Bar PVy
	if (projeccio == CAP) buffer = "       ";
	else if (pan) double2CString(tr_cpv.y);
	else double2CString(PVy);
	sss = _T("PVy=") + buffer;
	// Refrescar posici� PVy Status Bar
	GetStatusBar().SetPaneText(5, sss);

	// Status Bar PVz
	if (projeccio == CAP) buffer = "       ";
	else if (pan) double2CString(tr_cpv.z);
	else double2CString(PVz);
	sss = _T("PVz=") + buffer;
	// Refrescar posici� PVz Status Bar
	GetStatusBar().SetPaneText(6, sss);

	// Status Bar per indicar el modus de canvi de color (FONS o OBJECTE)
	sss = " ";
	if (sw_grid) sss = "GRID";
	else if (pan) sss = " PAN";
	else if (camera == CAM_NAVEGA) sss = " NAV";
	else if (sw_color) sss = " OBJ";
	else sss = "FONS";

	// Refrescar posici� Transformacions en Status Bar
	GetStatusBar().SetPaneText(7, sss);

	// Status Bar per indicar tipus de Transformaci� (TRAS, ROT, ESC)
	sss = " ";
	if (transf) {
		if (rota) sss = "ROT";
		else if (trasl) sss = "TRA";
		else if (escal) sss = "ESC";
	}
	else if ((!sw_grid) && (!pan) && (camera != CAM_NAVEGA))
	{	// Components d'intensitat de fons que varien per teclat
		if ((fonsR) && (fonsG) && (fonsB)) sss = " RGB";
		else if ((fonsR) && (fonsG)) sss = " RG ";
		else if ((fonsR) && (fonsB)) sss = " R   B";
		else if ((fonsG) && (fonsB)) sss = "   GB";
		else if (fonsR) sss = " R  ";
		else if (fonsG) sss = "   G ";
		else if (fonsB) sss = "      B";
	}
	// Refrescar posici� Transformacions en Status Bar
	GetStatusBar().SetPaneText(8, sss);

	// Status Bar dels par�metres de Transformaci�, Color i posicions de Robot i Cama
	sss = " ";
	if (transf)
	{
		if (rota)
		{
			buffer.Format(_T("%.1f"), TG.VRota.x);
			sss = _T("  ") + buffer + _T("   ");

			buffer.Format(_T("%.1f"), TG.VRota.y);
			sss = sss + buffer + _T("   ");

			buffer.Format(_T("%.1f"), TG.VRota.z);
			sss = sss + buffer;
		}
		else if (trasl)
		{
			buffer.Format(_T("%.1f"), TG.VTras.x);
			sss = _T("  ") + buffer + _T("   ");

			buffer.Format(_T("%.1f"), TG.VTras.y);
			sss = sss + buffer + _T("   ");

			buffer.Format(_T("%.1f"), TG.VTras.z);
			sss = sss + buffer;
		}
		else if (escal)
		{
			buffer.Format(_T("%.2f"), TG.VScal.x);
			sss = _T(" ") + buffer + _T("  ");

			buffer.Format(_T("%.2f"), TG.VScal.y);
			sss = sss + buffer + _T("  ");

			buffer.Format(_T("%.2f"), TG.VScal.x);
			sss = sss + buffer;
		}
	}
	else if ((!sw_grid) && (!pan) && (camera != CAM_NAVEGA))
	{	// Color fons
		if (!sw_color)
		{
			buffer.Format(_T("%.3f"), c_fons.r);
			sss = _T(" ") + buffer + _T("  ");

			buffer.Format(_T("%.3f"), c_fons.g);
			sss = sss + buffer + _T("  ");

			buffer.Format(_T("%.3f"), c_fons.b);
			sss = sss + buffer;
		}
		else
		{	// Color objecte
			buffer.Format(_T("%.3f"), col_obj.r);
			sss = _T(" ") + buffer + _T("  ");

			buffer.Format(_T("%.3f"), col_obj.g);
			sss = sss + buffer + _T("  ");

			buffer.Format(_T("%.3f"), col_obj.b);
			sss = sss + buffer;
		}
	}

	// Refrescar posici� dels par�metres de Transformaci�, Color i posicions de Robot i Cama
	GetStatusBar().SetPaneText(9, sss);

	// Status Bar per indicar el pas del Fractal
	if (objecte == O_FRACTAL)
	{
		buffer.Format(_T("%.1d"), pas);
		sss = _T("Pas=") + buffer;
	}
	else { sss = "          "; }

	// Refrescar posici� Transformacions en Status Bar
	GetStatusBar().SetPaneText(10, sss);
}

void CEntornVGIView::double2CString(double varf)
{
	double vdouble;
	vdouble = varf;
	if (abs(varf) < 1.0) buffer.Format(_T("%.5f"), varf);
	else if (abs(varf) < 99.0) buffer.Format(_T("%.4f"), varf);
	else if (abs(varf) < 999.0) buffer.Format(_T("%.3f"), varf);
	else if (abs(varf) < 9999.0) buffer.Format(_T("%.2f"), varf);
	else if (abs(varf) < 99999.0) buffer.Format(_T("%.1f"), varf);
	else buffer.Format(_T("%.0f"), varf);

}


/* ------------------------------------------------------------------------- */
/*                                  SHADERS                                  */
/* ------------------------------------------------------------------------- */

/*****************************************************************************
* initializeShaders()
*      Loads custom shader by specifying filename (expects frag/vert pair)
*****************************************************************************/
GLuint CEntornVGIView::initializeShaders(CString filename)
{
	GLuint v, f, p;
	char* vs = NULL, * fs = NULL;
	CString file_Vert, file_Frag;
	int info_log_length = 0, compile_result = 0, link_result = 0;
	FILE* fd;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	// Llegir Vertex Shader <filename>.vert
	file_Vert = filename + _T(".vert");
	char* fitxer_Vert = CString2Char(file_Vert);
	vs = textFileRead(fitxer_Vert);

	// Llegir Fragment Shader <filename>.vert
	file_Frag = filename + _T(".frag");
	char* fitxer_Frag = CString2Char(file_Frag);
	fs = textFileRead(fitxer_Frag);

	// Si s'han trobats els fitxers, procedir.
	if ((vs != NULL) && (fs != NULL))
	{
		const char* vv = vs;
		const char* ff = fs;

		glShaderSource(v, 1, &vv, NULL);
		glShaderSource(f, 1, &ff, NULL);

		free(vs);
		free(fs);

		// COMPILACI� VERTEX SHADER (VS)
		glCompileShader(v);
		glGetShaderiv(v, GL_COMPILE_STATUS, &compile_result);
		// Llista error de compilaci� del Vertex Shader
		if (compile_result == GL_FALSE) {
			GLint maxLength = 0;

			glGetShaderiv(v, GL_INFO_LOG_LENGTH, &maxLength);
			// The maxLength includes the NULL character
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(v, maxLength, &maxLength, &errorLog[0]);

			AfxMessageBox(_T("Error compilaci� Vertex Shader"));

			// Volcar missatges error a fitxer GLSL_Error.VERT
			if ((fd = fopen("GLSL_Error.VERT", "w")) == NULL)
			{
				AfxMessageBox(_T("GLSL_Error.VERT was not opened"));
				return GL_FALSE;
			}
			for (int i = 0; i <= maxLength; i = i++) fprintf(fd, "%c", errorLog[i]);
			fclose(fd);

			// Provide the infolog in whatever manor you deem best.
			// Exit with failure.
			glDeleteShader(v); // Don't leak the shader.
			return GL_FALSE;
		}
		else AfxMessageBox(_T("Compilacio Vertex Shader Ok"));

		// COMPILACI� FRAGMENT SHADER (FS)
		glCompileShader(f);
		glGetShaderiv(f, GL_COMPILE_STATUS, &compile_result);

		// Llista error de compilaci� del Vertex Shader
		if (compile_result == GL_FALSE) {
			GLint maxLength = 0;

			glGetShaderiv(f, GL_INFO_LOG_LENGTH, &maxLength);
			// The maxLength includes the NULL character
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(f, maxLength, &maxLength, &errorLog[0]);

			AfxMessageBox(_T("Error compilaci� Fragment Shader"));

			// Volcar missatges error a fitxer GLSL_Error.FRAG
			if ((fd = fopen("GLSL_Error.FRAG", "w")) == NULL)
			{
				AfxMessageBox(_T("GLSL_Error.FRAG was not opened"));
				return GL_FALSE;
			}
			for (int i = 0; i <= maxLength; i = i++) fprintf(fd, "%c", errorLog[i]);
			fclose(fd);

			// Provide the infolog in whatever manor you deem best.
			// Exit with failure.
			glDeleteShader(v); // Don't leak the shader.
			return GL_FALSE;
		}
		else AfxMessageBox(_T("Compilacio Fragment Shader Ok"));

		// LINKEDICI� SHADER PROGRAM (p)
		p = glCreateProgram();
		glAttachShader(p, v);	// Relaciona Vertex Shader al Program
		glAttachShader(p, f);	// relaciona Fragment Shader al Program

// Linkedici� del Program
		glLinkProgram(p);
		glGetProgramiv(p, GL_LINK_STATUS, &link_result);

		// Llista error de linkedici� del Shader Program
		if (link_result == GL_FALSE) {
			GLint maxLength = 0;

			glGetProgramiv(p, GL_INFO_LOG_LENGTH, &maxLength);
			// The maxLength includes the NULL character
			std::vector<GLchar> errorLog(maxLength);
			glGetProgramInfoLog(p, maxLength, &maxLength, &errorLog[0]);
			AfxMessageBox(_T("Error Linkedici� Shader Program"));

			// Volcar missatges error a fitxer GLSL_Error.LINK
			if ((fd = fopen("GLSL_Error.LINK", "w")) == NULL)
			{
				AfxMessageBox(_T("GLSL_Error.LINK was not opened"));
				return GL_FALSE;
			}
			for (int i = 0; i <= maxLength; i = i++) fprintf(fd, "%c", errorLog[i]);
			fclose(fd);

			glDeleteShader(v);		// Don't leak the shader.
			glDeleteShader(f);		// Don't leak the shader.
			glDeleteProgram(p);		// Don't leak the program.
			return GL_FALSE;		// Exit with failure.
		}
		else AfxMessageBox(_T("Linkedicio Shader Program OK"));

		// Activaci� del Programa
		glUseProgram(p);

		// Retorna refer�ncia al Program creat
		return p;
	}
	else return GL_FALSE;	// retornar 0 si no s'ha trobat el fitxer.
}


/*****************************************************************************
* loadFileShaders()
*      Loads custom shader by specifying filename (expects frag/vert pair)
*****************************************************************************/
GLuint CEntornVGIView::loadFileShaders(CString file_Vert, CString file_Frag)
{
	GLuint v, f, p;
	char* vs = NULL, * fs = NULL;
	//	CString file_Vert, file_Frag;
	int info_log_length = 0, compile_result = 0, link_result = 0;
	FILE* fd;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	//	file_Vert = filename + _T(".vert");
	char* fitxer_Vert = CString2Char(file_Vert);

	vs = textFileRead(fitxer_Vert);
	//vs = textFileRead(filename + ".vert");

	//	file_Frag = filename + _T(".frag");
	char* fitxer_Frag = CString2Char(file_Frag);

	fs = textFileRead(fitxer_Frag);
	//fs = textFileRead(filename + ".frag");

	if ((vs == NULL) || (fs == NULL)) return GL_FALSE;
	const char* vv = vs;
	const char* ff = fs;

	glShaderSource(v, 1, &vv, NULL);
	glShaderSource(f, 1, &ff, NULL);

	free(vs);
	free(fs);

	// COMPILACI� VERTEX SHADER (VS)
	glCompileShader(v);
	glGetShaderiv(v, GL_COMPILE_STATUS, &compile_result);
	// Llista error de compilaci�
	if (compile_result == GL_FALSE) {
		GLint maxLength = 0;

		glGetShaderiv(v, GL_INFO_LOG_LENGTH, &maxLength);
		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(v, maxLength, &maxLength, &errorLog[0]);
		AfxMessageBox(_T("Error compilaci� Vertex Shader"));

		// Volcar missatges error a fitxer GLSL_Error.VERT
		if ((fd = fopen("GLSL_Error.VERT", "w")) == NULL)
		{
			AfxMessageBox(_T("GLSL_Error.VERT was not opened"));
		}
		for (int i = 0; i <= maxLength; i = i++) fprintf(fd, "%c", errorLog[i]);
		fclose(fd);

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(v); // Don't leak the shader.
		return GL_FALSE;
	}

	// COMPILACI� FRAGMENT SHADER (FS)
	glCompileShader(f);
	glGetShaderiv(f, GL_COMPILE_STATUS, &compile_result);
	// Llista error de compilaci�
	if (compile_result == GL_FALSE) {
		GLint maxLength = 0;

		glGetShaderiv(f, GL_INFO_LOG_LENGTH, &maxLength);
		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(f, maxLength, &maxLength, &errorLog[0]);
		AfxMessageBox(_T("Error compilaci� Fragment Shader"));

		// Volcar missatges error a fitxer GLSL_Error.FRAG
		if ((fd = fopen("GLSL_Error.FRAG", "w")) == NULL)
		{
			AfxMessageBox(_T("GLSL_Error.FRAG was not opened"));
		}
		for (int i = 0; i <= maxLength; i = i++) fprintf(fd, "%c", errorLog[i]);
		fclose(fd);

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(f); // Don't leak the shader.
		return GL_FALSE;
	}

	// COMPILACI� FRAGMENT SHADER (FS)
	p = glCreateProgram();
	glAttachShader(p, v);
	glAttachShader(p, f);

	// LINKEDICI� SHADER PROGRAM (programID)
	glLinkProgram(p);
	glGetProgramiv(p, GL_LINK_STATUS, &link_result);

	// Llista error de compilaci� del Vertex Shader
	if (compile_result == GL_FALSE) {
		GLint maxLength = 0;

		glGetProgramiv(p, GL_INFO_LOG_LENGTH, &maxLength);
		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetProgramInfoLog(v, maxLength, &maxLength, &errorLog[0]);
		AfxMessageBox(_T("Error Linkedicio ShaderProgram"));

		// Volcar missatges error a fitxer GLSL_Error.LINK
		if ((fd = fopen("GLSL_Error.LINK", "w")) == NULL)
		{
			AfxMessageBox(_T("GLSL_Error.LINK was not opened"));
		}
		for (int i = 0; i <= maxLength; i = i++) fprintf(fd, "%c", errorLog[i]);
		fclose(fd);

		glDeleteShader(v);	// Don't leak the shader.
		glDeleteShader(f);	// Don't leak the shader.
		glDeleteProgram(p);	// Don't leak the program.
		return GL_FALSE;	// Exit with failure.
	}
	else AfxMessageBox(_T("Linkedicio Shader Program OK"));

	// Activaci� del Programa
	glUseProgram(p);

	// Retorna refer�ncia al Program creat
	return p;
}


char* CEntornVGIView::textFileRead(char* fn) {
	FILE* fp;
	char* content = NULL;
	int count = 0;
	if (fn != NULL) {
		fp = fopen(fn, "rt");
		if (fp != NULL) {
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);
			if (count > 0) {
				content = (char*)malloc(sizeof(char) * (count + 1));
				count = fread(content, sizeof(char), count, fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
		else {
			AfxMessageBox(_T("Vertex or Fragment Program not found"));
			return NULL;
		}
	}
	return content;
}

/*****************************************************************************
* releaseAllShaders()
*      Unload shaders, return to OpenGL fixed function.
*****************************************************************************/
void CEntornVGIView::releaseAllShaders()
{
	// Entorn VGI: Activaci� del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	glUseProgram(0);

	// Entorn VGI: Desactivaci� del contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
}


/* ------------------------------------------------------------------------- */
/*                           CONTROL DEL TECLAT                              */
/* ------------------------------------------------------------------------- */

// OnKeyDown: Funci� de tractament de teclat (funci� que es crida quan es prem una tecla)
//   PAR�METRES:
//    - nChar: Codi del caracter seleccionat
//    - nRepCnt: Nombre de vegades que s'ha apretat la tecla (acceleraci�)
//    - nFlags: Flags d'interrupci� activats.
void CEntornVGIView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Agregue aqu� su c�digo de controlador de mensajes o llame al valor predeterminado
	const double incr = 0.025f;
	double modul = 0;
	GLdouble vdir[3] = { 0, 0, 0 };

	if ((nChar == 'F') || (nChar == VK_F12)) this->OnVistaFullscreen();	// Activaci�-Desactivaci� Full Screen
	else if (objecte == C_BEZIER || objecte == C_BSPLINE || objecte == C_LEMNISCATA) Teclat_PasCorbes(nChar, nRepCnt);
	else if ((sw_grid) && ((grid.x) || (grid.y) || (grid.z))) Teclat_Grid(nChar, nRepCnt);
	else if (((nChar == 'G') || (nChar == 'g')) && ((grid.x) || (grid.y) || (grid.z))) sw_grid = !sw_grid;
	else if ((!pan) && (!transf) && (camera != CAM_NAVEGA))
	{
		if (!sw_color) Teclat_ColorFons(nChar, nRepCnt);
		else Teclat_ColorObjecte(nChar, nRepCnt);
	}
	else {
		if (transf)
		{
			if (rota) Teclat_TransRota(nChar, nRepCnt);
			else if (trasl) Teclat_TransTraslada(nChar, nRepCnt);
			else if (escal) Teclat_TransEscala(nChar, nRepCnt);
		}
		if (pan) Teclat_Pan(nChar, nRepCnt);
		else if (camera == CAM_NAVEGA) Teclat_Navega(nChar, nRepCnt);
		else if (!sw_color) Teclat_ColorFons(nChar, nRepCnt);
		else Teclat_ColorObjecte(nChar, nRepCnt);
	}
	if ((nChar == 'W') || (nChar == 'w')) {
		//pieces[2].moveUp(m_board.m_blocks);
		if (pieces[numPiece].moveUp(m_board.m_blocks)) {
			pieces[numPiece].ViewMoveUp();
		}

	}
	if ((nChar == 'S') || (nChar == 's')) {
		//pieces[2].moveDown();
		if (pieces[numPiece].moveDown(m_board.m_blocks)) {
			pieces[numPiece].ViewMoveDown();
		}

	}
	if ((nChar == 'A') || (nChar == 'a')) {
		if (pieces[numPiece].moveLeft(m_board.m_blocks)) {
			pieces[numPiece].ViewMoveLeft();
		}

	}
	if ((nChar == 'D') || (nChar == 'd')) {
		if (pieces[numPiece].moveRight(m_board.m_blocks)) {
			pieces[numPiece].ViewMoveRight();
		}
	}
	if ((nChar == 'Q') || (nChar == 'q')) {
		if (pieces[numPiece].rotateLeftOverY(m_board.m_blocks)) {
			pieces[numPiece].ViewRotateLeftOverY();
		}
	}
	if ((nChar == 'E') || (nChar == 'e')) {
		if (pieces[numPiece].rotateRightOverY(m_board.m_blocks)) {
			pieces[numPiece].ViewRotateRightOverY();
		}
	}
	if ((nChar == 'P') || (nChar == 'p')) {
		if (tetris) {
			KillTimer(WM_TIMER);
		}
		else {
			SetTimer(WM_TIMER, 1000, NULL);
		}

		tetris = !tetris;
	}
	if ((nChar == 'z') || (nChar == 'Z'))
	{
		if (pieces[numPiece].cauPeca(m_board.m_blocks)) {
			pieces[numPiece].ViewcauPeca();
		}
		else {
			numPiece = rand() % (sizeof(pieces) / sizeof(pieces[0]));
			pieces[numPiece].posIni();
		}


	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CEntornVGIView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Agregue aqu� su c�digo de controlador de mensajes o llame al valor predeterminado

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}


// Teclat_ColorObjecte: Teclat pels canvis de color de l'objecte per teclat.
void CEntornVGIView::Teclat_ColorObjecte(UINT nChar, UINT nRepCnt)
{
	const double incr = 0.025f;

	// FRACTAL: Canvi resoluci� del fractal pe tecles '+' i'-'
	if (objecte == O_FRACTAL)
	{
		if (nChar == 109) // Car�cter '-' 
		{
			pas = pas * 2;
			if (pas > 64) pas = 64;
			sw_il = true;
		}
		else if (nChar == 107) // Car�cter '+' 
		{
			pas = pas / 2;
			if (pas < 1) pas = 1;
			sw_il = true;
		}
	}
	//	else 
	if (nChar == VK_DOWN)
	{
		if (fonsR) {
			col_obj.r -= nRepCnt * incr;
			if (col_obj.r < 0.0) col_obj.r = 0.0;
		}
		if (fonsG) {
			col_obj.g -= nRepCnt * incr;
			if (col_obj.g < 0.0) col_obj.g = 0.0;
		}
		if (fonsB) {
			col_obj.b -= nRepCnt * incr;
			if (col_obj.b < 0.0) col_obj.b = 0.0;
		}
	}
	else if (nChar == VK_UP)
	{
		if (fonsR) {
			col_obj.r += nRepCnt * incr;
			if (col_obj.r > 1.0) col_obj.r = 1.0;
		}
		if (fonsG) {
			col_obj.g += nRepCnt * incr;
			if (col_obj.g > 1.0) col_obj.g = 1.0;
		}
		if (fonsB) {
			col_obj.b += nRepCnt * incr;
			if (col_obj.b > 1.0) col_obj.b = 1.0;
		}
	}
	else if (nChar == VK_SPACE)
	{
		if ((fonsR) && (fonsG) && (fonsB)) {
			fonsG = false;
			fonsB = false;
		}
		else if ((fonsR) && (fonsG)) {
			fonsG = false;
			fonsB = true;
		}
		else if ((fonsR) && (fonsB)) {
			fonsR = false;
			fonsG = true;
		}
		else if ((fonsG) && (fonsB)) fonsR = true;
		else if (fonsR) {
			fonsR = false;
			fonsG = true;
		}
		else if (fonsG) {
			fonsG = false;
			fonsB = true;
		}
		else if (fonsB) {
			fonsR = true;
			fonsG = true;
			fonsB = false;
		}
	}
	else if ((nChar == 'o') || (nChar == 'O')) sw_color = true;
	else if ((nChar == 'b') || (nChar == 'B')) sw_color = false;

}


// Teclat_ColorFons: Teclat pels canvis del color de fons.
void CEntornVGIView::Teclat_ColorFons(UINT nChar, UINT nRepCnt)
{
	const double incr = 0.025f;

	// FRACTAL: Canvi resoluci� del fractal pe tecles '+' i'-'
	if (objecte == O_FRACTAL)
	{
		if (nChar == 109) // Car�cter '-' 
		{
			pas = pas * 2;
			if (pas > 64) pas = 64;
			sw_il = true;
		}
		else if (nChar == 107) // Car�cter '+' 
		{
			pas = pas / 2;
			if (pas < 1) pas = 1;
			sw_il = true;
		}
	}
	//	else 
	if (nChar == VK_DOWN) {
		if (fonsR) {
			c_fons.r -= nRepCnt * incr;
			if (c_fons.r < 0.0) c_fons.r = 0.0;
		}
		if (fonsG) {
			c_fons.g -= nRepCnt * incr;
			if (c_fons.g < 0.0) c_fons.g = 0.0;
		}
		if (fonsB) {
			c_fons.b -= nRepCnt * incr;
			if (c_fons.b < 0.0) c_fons.b = 0.0;
		}
	}
	else if (nChar == VK_UP) {
		if (fonsR) {
			c_fons.r += nRepCnt * incr;
			if (c_fons.r > 1.0) c_fons.r = 1.0;
		}
		if (fonsG) {
			c_fons.g += nRepCnt * incr;
			if (c_fons.g > 1.0) c_fons.g = 1.0;
		}
		if (fonsB) {
			c_fons.b += nRepCnt * incr;
			if (c_fons.b > 1.0) c_fons.b = 1.0;
		}
	}
	else if (nChar == VK_SPACE) {
		if ((fonsR) && (fonsG) && (fonsB)) {
			fonsG = false;
			fonsB = false;
		}
		else if ((fonsR) && (fonsG)) {
			fonsG = false;
			fonsB = true;
		}
		else if ((fonsR) && (fonsB)) {
			fonsR = false;
			fonsG = true;
		}
		else if ((fonsG) && (fonsB)) fonsR = true;
		else if (fonsR) {
			fonsR = false;
			fonsG = true;
		}
		else if (fonsG) {
			fonsG = false;
			fonsB = true;
		}
		else if (fonsB) {
			fonsR = true;
			fonsG = true;
			fonsB = false;
		}
	}
	else if ((nChar == 'o') || (nChar == 'O')) sw_color = true;
	else if ((nChar == 'b') || (nChar == 'B')) sw_color = false;
}

// Teclat_Navega: Teclat pels moviments de navegaci�.
void CEntornVGIView::Teclat_Navega(UINT nChar, UINT nRepCnt)
{
	GLdouble vdir[3] = { 0, 0, 0 };
	double modul = 0;

	// Entorn VGI: Controls de moviment de navegaci�
	vdir[0] = n[0] - opvN.x;
	vdir[1] = n[1] - opvN.y;
	vdir[2] = n[2] - opvN.z;
	modul = sqrt(vdir[0] * vdir[0] + vdir[1] * vdir[1] + vdir[2] * vdir[2]);
	vdir[0] = vdir[0] / modul;
	vdir[1] = vdir[1] / modul;
	vdir[2] = vdir[2] / modul;
	switch (nChar)
	{
		// Tecla cursor amunt
	case VK_UP:
		opvN.x += nRepCnt * fact_pan * vdir[0];
		opvN.y += nRepCnt * fact_pan * vdir[1];
		n[0] += nRepCnt * fact_pan * vdir[0];
		n[1] += nRepCnt * fact_pan * vdir[1];
		break;

		// Tecla cursor avall
	case VK_DOWN:
		opvN.x -= nRepCnt * fact_pan * vdir[0];
		opvN.y -= nRepCnt * fact_pan * vdir[1];
		n[0] -= nRepCnt * fact_pan * vdir[0];
		n[1] -= nRepCnt * fact_pan * vdir[1];
		break;

		// Tecla cursor esquerra
	case VK_LEFT:
		angleZ = +nRepCnt * fact_pan;
		n[0] = n[0] - opvN.x;
		n[1] = n[1] - opvN.y;
		n[0] = n[0] * cos(angleZ * PI / 180) - n[1] * sin(angleZ * PI / 180);
		n[1] = n[0] * sin(angleZ * PI / 180) + n[1] * cos(angleZ * PI / 180);
		n[0] = n[0] + opvN.x;
		n[1] = n[1] + opvN.y;
		break;

		// Tecla cursor dret
	case VK_RIGHT:
		angleZ = 360 - nRepCnt * fact_pan;
		n[0] = n[0] - opvN.x;
		n[1] = n[1] - opvN.y;
		n[0] = n[0] * cos(angleZ * PI / 180) - n[1] * sin(angleZ * PI / 180);
		n[1] = n[0] * sin(angleZ * PI / 180) + n[1] * cos(angleZ * PI / 180);
		n[0] = n[0] + opvN.x;
		n[1] = n[1] + opvN.y;
		break;

		// Tecla Inicio
	case VK_HOME:
		opvN.z += nRepCnt * fact_pan;
		n[2] += nRepCnt * fact_pan;
		break;

		// Tecla Fin
	case VK_END:
		opvN.z -= nRepCnt * fact_pan;
		n[2] -= nRepCnt * fact_pan;
		break;

		// Tecla PgUp
	case VK_PRIOR:
		fact_pan /= 2;
		if (fact_pan < 1) fact_pan = 1;
		break;

		// Tecla PgDown
	case VK_NEXT:
		fact_pan *= 2;
		if (fact_pan > 2048) fact_pan = 2048;
		break;

	default:
		break;
	}
}


// Teclat_Pan: Teclat pels moviments de Pan.
void CEntornVGIView::Teclat_Pan(UINT nChar, UINT nRepCnt)
{
	switch (nChar)
	{
		// Tecla cursor amunt
	case VK_UP:
		tr_cpv.y -= nRepCnt * fact_pan;
		if (tr_cpv.y < -100000) tr_cpv.y = 100000;
		break;

		// Tecla cursor avall
	case VK_DOWN:
		tr_cpv.y += nRepCnt * fact_pan;
		if (tr_cpv.y > 100000) tr_cpv.y = 100000;
		break;

		// Tecla cursor esquerra
	case VK_LEFT:
		tr_cpv.x += nRepCnt * fact_pan;
		if (tr_cpv.x > 100000) tr_cpv.x = 100000;
		break;

		// Tecla cursor dret
	case VK_RIGHT:
		tr_cpv.x -= nRepCnt * fact_pan;
		if (tr_cpv.x < -100000) tr_cpv.x = 100000;
		break;

		// Tecla PgUp
	case VK_PRIOR:
		fact_pan /= 2;
		if (fact_pan < 0.125) fact_pan = 0.125;
		break;

		// Tecla PgDown
	case VK_NEXT:
		fact_pan *= 2;
		if (fact_pan > 2048) fact_pan = 2048;
		break;

		// Tecla Insert: Fixar el despla�ament de pantalla (pan)
	case VK_INSERT:
		// Acumular despla�aments de pan (tr_cpv) en variables fixes (tr_cpvF).
		tr_cpvF.x += tr_cpv.x;		tr_cpv.x = 0.0;
		if (tr_cpvF.x > 100000) tr_cpvF.y = 100000;
		tr_cpvF.y += tr_cpv.y;		tr_cpv.y = 0.0;
		if (tr_cpvF.y > 100000) tr_cpvF.y = 100000;
		tr_cpvF.z += tr_cpv.z;		tr_cpv.z = 0.0;
		if (tr_cpvF.z > 100000) tr_cpvF.z = 100000;
		break;

		// Tecla Delete: Inicialitzar el despla�ament de pantalla (pan)
	case VK_DELETE:
		// Inicialitzar els valors de pan tant de la variable tr_cpv com de la tr_cpvF.
		tr_cpv.x = 0.0;			tr_cpv.y = 0.0;			tr_cpv.z = 0.0;
		tr_cpvF.x = 0.0;		tr_cpvF.y = 0.0;		tr_cpvF.z = 0.0;
		break;

	default:
		break;
	}
}

// Teclat_TransEscala: Teclat pels canvis del valor d'escala per X,Y,Z.
void CEntornVGIView::Teclat_TransEscala(UINT nChar, UINT nRepCnt)
{
	switch (nChar)
	{
		// Modificar vector d'Escalatge per teclat (actiu amb Escalat �nicament)
			// Tecla '+' (augmentar tot l'escalat)
	case 107:
		TG.VScal.x = TG.VScal.x * 2;
		if (TG.VScal.x > 8192) TG.VScal.x = 8192;
		TG.VScal.y = TG.VScal.y * 2;
		if (TG.VScal.y > 8192) TG.VScal.y = 8192;
		TG.VScal.z = TG.VScal.z * 2;
		if (TG.VScal.z > 8192) TG.VScal.z = 8192;
		break;

		// Tecla '-' (disminuir tot l'escalat)
	case 109:
		TG.VScal.x = TG.VScal.x / 2;
		if (TG.VScal.x < 0.25) TG.VScal.x = 0.25;
		TG.VScal.y = TG.VScal.y / 2;
		if (TG.VScal.y < 0.25) TG.VScal.y = 0.25;
		TG.VScal.z = TG.VScal.z / 2;
		if (TG.VScal.z < 0.25) TG.VScal.z = 0.25;
		break;

		// Tecla cursor amunt ('8')
	case 104:
		TG.VScal.x = TG.VScal.x * 2;
		if (TG.VScal.x > 8192) TG.VScal.x = 8192;
		break;

		// Tecla cursor avall ('2')
	case 98:
		TG.VScal.x = TG.VScal.x / 2;
		if (TG.VScal.x < 0.25) TG.VScal.x = 0.25;
		break;

		// Tecla cursor esquerra ('4')
	case 100:
		TG.VScal.y = TG.VScal.y / 2;
		if (TG.VScal.y < 0.25) TG.VScal.y = 0.25;
		break;

		// Tecla cursor dret ('6')
	case 102:
		TG.VScal.y = TG.VScal.y * 2;
		if (TG.VScal.y > 8192) TG.VScal.y = 8192;
		break;

		// Tecla HOME ('7')
	case 103:
		TG.VScal.z = TG.VScal.z * 2;
		if (TG.VScal.z > 8192) TG.VScal.z = 8192;
		break;

		// Tecla END ('1')
	case 97:
		TG.VScal.z = TG.VScal.z / 2;
		if (TG.VScal.z < 0.25) TG.VScal.z = 0.25;
		break;

		// Tecla INSERT
	case VK_INSERT:
		// Acumular transformacions Geom�triques (variable TG) i de pan en variables fixes (variable TGF)
		TGF.VScal.x *= TG.VScal.x;	TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
		if (TGF.VScal.x > 8192)		TGF.VScal.x = 8192;
		if (TGF.VScal.y > 8192)		TGF.VScal.y = 8192;
		if (TGF.VScal.z > 8192)		TGF.VScal.z = 8192;
		TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
		TGF.VRota.x += TG.VRota.x;	TGF.VRota.y += TG.VRota.y; TGF.VRota.z += TG.VRota.z;
		if (TGF.VRota.x >= 360)		TGF.VRota.x -= 360; 		if (TGF.VRota.x < 0) TGF.VRota.x += 360;
		if (TGF.VRota.y >= 360)		TGF.VRota.y -= 360;		if (TGF.VRota.y < 0) TGF.VRota.y += 360;
		if (TGF.VRota.z >= 360)		TGF.VRota.z -= 360;		if (TGF.VRota.z < 0) TGF.VRota.z += 360;
		TG.VRota.x = 0.0;				TG.VRota.y = 0.0;					TG.VRota.z = 0.0;
		TGF.VTras.x += TG.VTras.x;	TGF.VTras.y += TG.VTras.y; TGF.VTras.z += TG.VTras.z;
		if (TGF.VTras.x < -100000)		TGF.VTras.x = 100000;		if (TGF.VTras.x > 10000) TGF.VTras.x = 100000;
		if (TGF.VTras.y < -100000)		TGF.VTras.y = 100000;		if (TGF.VTras.y > 10000) TGF.VTras.y = 100000;
		if (TGF.VTras.z < -100000)		TGF.VTras.z = 100000;		if (TGF.VTras.z > 10000) TGF.VTras.z = 100000;
		TG.VTras.x = 0.0;		TG.VTras.y = 0.0;		TG.VTras.z = 0.0;
		break;

		// Tecla Delete: Esborrar les Transformacions Geom�triques Calculades
	case VK_DELETE:
		// Inicialitzar els valors de transformacions Geom�triques i de pan en variables fixes.
		TGF.VScal.x = 1.0;		TGF.VScal.y = 1.0;;		TGF.VScal.z = 1.0;
		TG.VScal.x = 1.0;		TG.VScal.y = 1.0;		TG.VScal.z = 1.0;
		TGF.VRota.x = 0.0;		TGF.VRota.y = 0.0;		TGF.VRota.z = 0.0;
		TG.VRota.x = 0.0;		TG.VRota.y = 0.0;		TG.VRota.z = 0.0;
		TGF.VTras.x = 0.0;		TGF.VTras.y = 0.0;		TGF.VTras.z = 0.0;
		TG.VTras.x = 0.0;		TG.VTras.y = 0.0;		TG.VTras.z = 0.0;
		break;

	default:
		break;
	}
}

// Teclat_TransRota: Teclat pels canvis del valor del vector de l'angle de rotaci� per X,Y,Z.
void CEntornVGIView::Teclat_TransRota(UINT nChar, UINT nRepCnt)
{
	switch (nChar)
	{
		// Tecla cursor amunt ('8')
	case 104:
		TG.VRota.x += nRepCnt * fact_Rota;
		if (TG.VRota.x >= 360) TG.VRota.x -= 360;
		break;

		// Tecla cursor avall ('2')
	case 98:
		TG.VRota.x -= nRepCnt * fact_Rota;
		if (TG.VRota.x < 0.0) TG.VRota.x += 360;
		break;

		// Tecla cursor esquerra ('4')
	case 100:
		TG.VRota.y -= nRepCnt * fact_Rota;
		if (TG.VRota.y < 0.0) TG.VRota.y += 360;
		break;

		// Tecla cursor dret ('6')
	case 102:
		TG.VRota.y += nRepCnt * fact_Rota;
		if (TG.VRota.y >= 360) TG.VRota.y -= 360;
		break;

		// Tecla Return Numpad
	case VK_RETURN:
		TG.VRota.y += nRepCnt * fact_Rota;
		if (TG.VRota.y >= 360.0) TG.VRota.y -= 360.0;
		break;

		// Tecla HOME ('7')
	case 103:
		TG.VRota.z += nRepCnt * fact_Rota;
		if (TG.VRota.z >= 360) TG.VRota.z -= 360;
		break;

		// Tecla END ('1')
	case 97:
		TG.VRota.z -= nRepCnt * fact_Rota;
		if (TG.VRota.z < 0.0) TG.VRota.z += 360;
		break;

		// Tecla PgUp ('9')
	case 105:
		fact_Rota /= 2;
		if (fact_Rota < 1) fact_Rota = 1.0;
		break;

		// Tecla PgDown ('3')
	case 99:
		fact_Rota *= 2;
		if (fact_Rota > 90) fact_Rota = 90;
		break;

		// Modificar vector d'Escalatge per teclat (actiu amb Rotaci�)
			// Tecla '+' (augmentar escalat)
	case 107:
		TG.VScal.x = TG.VScal.x * 2;
		if (TG.VScal.x > 8192) TG.VScal.x = 8192;
		TG.VScal.y = TG.VScal.y * 2;
		if (TG.VScal.y > 8192) TG.VScal.y = 8192;
		TG.VScal.z = TG.VScal.z * 2;
		if (TG.VScal.z > 8192) TG.VScal.z = 8192;
		break;

		// Tecla '-' (disminuir escalat)
	case 109:
		TG.VScal.x = TG.VScal.x / 2;
		if (TG.VScal.x < 0.25) TG.VScal.x = 0.25;
		TG.VScal.y = TG.VScal.y / 2;
		if (TG.VScal.y < 0.25) TG.VScal.y = 0.25;
		TG.VScal.z = TG.VScal.z / 2;
		if (TG.VScal.z < 0.25) TG.VScal.z = 0.25;
		break;

		// Tecla Insert: Acumular transformacions Geom�triques (variable TG) i de pan en variables fixes (variable TGF)
	case VK_INSERT:
		TGF.VScal.x *= TG.VScal.x;	TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
		if (TGF.VScal.x > 8192)		TGF.VScal.x = 8192;
		if (TGF.VScal.y > 8192)		TGF.VScal.y = 8192;
		if (TGF.VScal.z > 8192)		TGF.VScal.z = 8192;
		TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
		TGF.VRota.x += TG.VRota.x;	TGF.VRota.y += TG.VRota.y; TGF.VRota.z += TG.VRota.z;
		if (TGF.VRota.x >= 360)		TGF.VRota.x -= 360; 		if (TGF.VRota.x < 0) TGF.VRota.x += 360;
		if (TGF.VRota.y >= 360)		TGF.VRota.y -= 360;		if (TGF.VRota.y < 0) TGF.VRota.y += 360;
		if (TGF.VRota.z >= 360)		TGF.VRota.z -= 360;		if (TGF.VRota.z < 0) TGF.VRota.z += 360;
		TG.VRota.x = 0.0;				TG.VRota.y = 0.0;					TG.VRota.z = 0.0;
		TGF.VTras.x += TG.VTras.x;	TGF.VTras.y += TG.VTras.y; TGF.VTras.z += TG.VTras.z;
		if (TGF.VTras.x < -100000)		TGF.VTras.x = 100000;		if (TGF.VTras.x > 10000) TGF.VTras.x = 100000;
		if (TGF.VTras.y < -100000)		TGF.VTras.y = 100000;		if (TGF.VTras.y > 10000) TGF.VTras.y = 100000;
		if (TGF.VTras.z < -100000)		TGF.VTras.z = 100000;		if (TGF.VTras.z > 10000) TGF.VTras.z = 100000;
		TG.VTras.x = 0.0;		TG.VTras.y = 0.0;		TG.VTras.z = 0.0;
		break;

		// Tecla Delete: Esborrar les Transformacions Geom�triques Calculades
	case VK_DELETE:
		// Inicialitzar els valors de transformacions Geom�triques i de pan en variables fixes.
		TGF.VScal.x = 1.0;	TGF.VScal.y = 1.0;;	TGF.VScal.z = 1.0;
		TG.VScal.x = 1.0;		TG.VScal.y = 1.0;		TG.VScal.z = 1.0;
		TGF.VRota.x = 0.0;	TGF.VRota.y = 0.0;	TGF.VRota.z = 0.0;
		TG.VRota.x = 0.0;		TG.VRota.y = 0.0;		TG.VRota.z = 0.0;
		TGF.VTras.x = 0.0;	TGF.VTras.y = 0.0;	TGF.VTras.z = 0.0;
		TG.VTras.x = 0.0;		TG.VTras.y = 0.0;		TG.VTras.z = 0.0;
		break;

		// Tecla Espaiador
	case VK_SPACE:
		rota = !rota;
		trasl = !trasl;
		break;

	default:
		break;
	}
}


// Teclat_TransTraslada: Teclat pels canvis del valor de traslaci� per X,Y,Z.
void CEntornVGIView::Teclat_TransTraslada(UINT nChar, UINT nRepCnt)
{
	GLdouble vdir[3] = { 0, 0, 0 };
	double modul = 0;

	switch (nChar)
	{
		// Tecla cursor amunt ('8')
	case 104:
		TG.VTras.x -= nRepCnt * fact_Tras;
		if (TG.VTras.x < -100000) TG.VTras.x = 100000;
		break;

		// Tecla cursor avall ('2')
	case 98:
		TG.VTras.x += nRepCnt * fact_Tras;
		if (TG.VTras.x > 10000) TG.VTras.x = 100000;
		break;

		// Tecla cursor esquerra ('4')
	case 100:
		TG.VTras.y -= nRepCnt * fact_Tras;
		if (TG.VTras.y < -100000) TG.VTras.y = -100000;
		break;

		// Tecla cursor dret ('6')
	case 102:
		TG.VTras.y += nRepCnt * fact_Tras;
		if (TG.VTras.y > 100000) TG.VTras.y = 100000;
		break;

		// Tecla HOME ('7')
	case 103:
		TG.VTras.z += nRepCnt * fact_Tras;
		if (TG.VTras.z > 100000) TG.VTras.z = 100000;
		break;

		// Tecla END ('1')
	case 97:
		TG.VTras.z -= nRepCnt * fact_Tras;
		if (TG.VTras.z < -100000) TG.VTras.z = -100000;
		break;

		// Tecla PgUp ('9')
	case 105:
		fact_Tras /= 2;
		if (fact_Tras < 1) fact_Tras = 1;
		break;

		// Tecla PgDown ('3')
	case 99:
		fact_Tras *= 2;
		if (fact_Tras > 100000) fact_Tras = 100000;
		break;

		// Modificar vector d'Escalatge per teclat (actiu amb Traslaci�)
			// Tecla '+' (augmentar escalat)
	case 107:
		TG.VScal.x = TG.VScal.x * 2;
		if (TG.VScal.x > 8192) TG.VScal.x = 8192;
		TG.VScal.y = TG.VScal.y * 2;
		if (TG.VScal.y > 8192) TG.VScal.y = 8192;
		TG.VScal.z = TG.VScal.z * 2;
		if (TG.VScal.z > 8192) TG.VScal.z = 8192;
		break;

		// Tecla '-' (disminuir escalat)
	case 109:
		TG.VScal.x = TG.VScal.x / 2;
		if (TG.VScal.x < 0.25) TG.VScal.x = 0.25;
		TG.VScal.y = TG.VScal.y / 2;
		if (TG.VScal.y < 0.25) TG.VScal.y = 0.25;
		TG.VScal.z = TG.VScal.z / 2;
		if (TG.VScal.z < 0.25) TG.VScal.z = 0.25;
		break;

		// Tecla INSERT
	case VK_INSERT:
		// Acumular transformacions Geom�triques (variable TG) i de pan en variables fixes (variable TGF)
		TGF.VScal.x *= TG.VScal.x;	TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
		if (TGF.VScal.x > 8192)		TGF.VScal.x = 8192;
		if (TGF.VScal.y > 8192)		TGF.VScal.y = 8192;
		if (TGF.VScal.z > 8192)		TGF.VScal.z = 8192;
		TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
		TGF.VRota.x += TG.VRota.x;	TGF.VRota.y += TG.VRota.y; TGF.VRota.z += TG.VRota.z;
		if (TGF.VRota.x >= 360)		TGF.VRota.x -= 360; 		if (TGF.VRota.x < 0) TGF.VRota.x += 360;
		if (TGF.VRota.y >= 360)		TGF.VRota.y -= 360;		if (TGF.VRota.y < 0) TGF.VRota.y += 360;
		if (TGF.VRota.z >= 360)		TGF.VRota.z -= 360;		if (TGF.VRota.z < 0) TGF.VRota.z += 360;
		TG.VRota.x = 0.0;				TG.VRota.y = 0.0;					TG.VRota.z = 0.0;
		TGF.VTras.x += TG.VTras.x;	TGF.VTras.y += TG.VTras.y; TGF.VTras.z += TG.VTras.z;
		if (TGF.VTras.x < -100000)		TGF.VTras.x = 100000;		if (TGF.VTras.x > 10000) TGF.VTras.x = 100000;
		if (TGF.VTras.y < -100000)		TGF.VTras.y = 100000;		if (TGF.VTras.y > 10000) TGF.VTras.y = 100000;
		if (TGF.VTras.z < -100000)		TGF.VTras.z = 100000;		if (TGF.VTras.z > 10000) TGF.VTras.z = 100000;
		TG.VTras.x = 0.0;		TG.VTras.y = 0.0;		TG.VTras.z = 0.0;
		break;

		// Tecla Delete: Esborrar les Transformacions Geom�triques Calculades
	case VK_DELETE:
		// Inicialitzar els valors de transformacions Geom�triques i de pan en variables fixes.
		TGF.VScal.x = 1.0;		TGF.VScal.y = 1.0;;		TGF.VScal.z = 1.0;
		TG.VScal.x = 1.0;		TG.VScal.y = 1.0;		TG.VScal.z = 1.0;
		TGF.VRota.x = 0.0;		TGF.VRota.y = 0.0;		TGF.VRota.z = 0.0;
		TG.VRota.x = 0.0;		TG.VRota.y = 0.0;		TG.VRota.z = 0.0;
		TGF.VTras.x = 0.0;		TGF.VTras.y = 0.0;		TGF.VTras.z = 0.0;
		TG.VTras.x = 0.0;		TG.VTras.y = 0.0;		TG.VTras.z = 0.0;
		break;

		// Tecla Espaiador
	case VK_SPACE:
		rota = !rota;
		trasl = !trasl;
		break;

	default:
		break;
	}
}


// Teclat_Grid: Teclat pels despla�aments dels gridXY, gridXZ i gridYZ.
void CEntornVGIView::Teclat_Grid(UINT nChar, UINT nRepCnt)
{
	switch (nChar)
	{
		// Key Up cursor ('8')
	case 104:
		hgrid.x -= nRepCnt * PAS_GRID;
		break;

		// Key Down cursor ('2')
	case 98:
		hgrid.x += nRepCnt * PAS_GRID;
		break;

		// Key Left cursor ('4')
	case 100:
		hgrid.y -= nRepCnt * PAS_GRID;
		break;

		// Key Right cursor ('6')
	case 102:
		hgrid.y += nRepCnt * PAS_GRID;
		break;

		// Key HOME ('7')
	case 103:
		hgrid.z += nRepCnt * PAS_GRID;
		break;

		// Key END ('1')
	case 97:
		hgrid.z -= nRepCnt * PAS_GRID;
		break;

		// Key grid ('G')
	case 'G':
		sw_grid = !sw_grid;
		break;

		/*
		// Key grid ('g')
		case 'g':
		sw_grid = !sw_grid;
		break;
		*/

	default:
		break;
	}
}


// Teclat_PasCorbes: Teclat per incrementar-Decrementar el pas de dibuix de les corbes (pas_CS).
void CEntornVGIView::Teclat_PasCorbes(UINT nChar, UINT nRepCnt)
{
	switch (nChar)
	{
		// Tecla '+' (incrementar pas_CS)
	case 107:
		pas_CS = pas_CS * 2.0;
		if (pas_CS > 0.5) pas_CS = 0.5;
		break;

		// Tecla '-' (decrementar pas_CS)
	case 109:
		pas_CS = pas_CS / 2;
		if (pas_CS < 0.0125) pas_CS = 0.00625;
		break;

		// Tecla PgUp ('9') (incrementar pas_CS)
	case VK_PRIOR:
		pas_CS = pas_CS * 2.0;
		if (pas_CS > 0.5) pas_CS = 0.5;
		break;

		// Tecla PgDown ('3') (decrementar pas_CS)
	case VK_NEXT:
		pas_CS = pas_CS / 2;
		if (pas_CS < 0.0125) pas_CS = 0.00625;
		break;

	default:
		if (transf)
		{
			if (rota) Teclat_TransRota(nChar, nRepCnt);
			else if (trasl) Teclat_TransTraslada(nChar, nRepCnt);
			else if (escal) Teclat_TransEscala(nChar, nRepCnt);
		}
		if (pan) Teclat_Pan(nChar, nRepCnt);
		else if (camera == CAM_NAVEGA) Teclat_Navega(nChar, nRepCnt);
		if (!sw_color) Teclat_ColorFons(nChar, nRepCnt);
		else Teclat_ColorObjecte(nChar, nRepCnt);
		break;
	}
}


/* ------------------------------------------------------------------------- */
/*                           CONTROL DEL RATOLI                              */
/* ------------------------------------------------------------------------- */

// OnLButtonDown: Funci� que es crida quan s'apreta el bot� esquerra del mouse.
void CEntornVGIView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Agregue aqu� su c�digo de controlador de mensajes o llame al valor predeterminado

	// Entorn VGI: Detectem en quina posici� s'ha apretat el bot� esquerra del
	//				mouse i ho guardem a la variable m_PosEAvall i activem flag m_ButoEAvall
	m_ButoEAvall = true;
	m_PosEAvall = point;
	m_EsfeEAvall = OPV;

	CView::OnLButtonDown(nFlags, point);
}


// OnLButtonDown: Funci� que es crida quan deixem d'apretar el bot� esquerra del mouse.
void CEntornVGIView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Agregue aqu� su c�digo de controlador de mensajes o llame al valor predeterminado
	// Entorn VGI: Desactivem flag m_ButoEAvall quan deixem d'apretar bot� esquerra del mouse.
	m_ButoEAvall = false;

	// OPCI� VISTA-->SAT�LIT: C�lcul increment despla�ament del Punt de Vista
	if ((satelit) && (projeccio != ORTO))
	{	//m_EsfeIncEAvall.R = m_EsfeEAvall.R - OPV.R;
		m_EsfeIncEAvall.alfa = 0.01f * (OPV.alfa - m_EsfeEAvall.alfa); //if (abs(m_EsfeIncEAvall.alfa)<0.01) { if ((m_EsfeIncEAvall.alfa)>0.0) m_EsfeIncEAvall.alfa = 0.01 else m_EsfeIncEAvall.alfa=0.01}
		m_EsfeIncEAvall.beta = 0.01f * (OPV.beta - m_EsfeEAvall.beta);
		if (abs(m_EsfeIncEAvall.beta) < 0.01) {
			if ((m_EsfeIncEAvall.beta) > 0.0) m_EsfeIncEAvall.beta = 0.01;
			else m_EsfeIncEAvall.beta = 0.01;
		}
		if ((m_EsfeIncEAvall.R == 0.0) && (m_EsfeIncEAvall.alfa == 0.0) && (m_EsfeIncEAvall.beta == 0.0)) KillTimer(WM_TIMER);
		else SetTimer(WM_TIMER, 10, NULL);
	}

	CView::OnLButtonUp(nFlags, point);
}


// OnLButtonDown: Funci� que es crida quan s'apreta el bot� dret del mouse.
void CEntornVGIView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Agregue aqu� su c�digo de controlador de mensajes o llame al valor predeterminado
	// Entorn VGI: Detectem en quina posici� s'ha apretat el bot� esquerra del
	//				mouse i ho guardem a la variable m_PosEAvall i activem flag m_ButoDAvall
	m_ButoDAvall = true;
	m_PosDAvall = point;

	CView::OnRButtonDown(nFlags, point);
}


// OnLButtonDown: Funci� que es crida quan deixem d'apretar el bot� dret del mouse.
void CEntornVGIView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	// Entorn VGI: Desactivem flag m_ButoEAvall quan deixem d'apretar bot� esquerra del mouse.
	m_ButoDAvall = false;

	//	ClientToScreen(&point);
	//	OnContextMenu(this, point);

	// Si fullscreen afegir men� per a restaurar Pantalla Normal en posici� del cursor.
	if (fullscreen) {
		ClientToScreen(&point);
		if (ContextMenu->GetSubMenu(0)) {
			CMenu* pSubMenu = ContextMenu->GetSubMenu(0);

			if (fullscreen)	pSubMenu->CheckMenuItem(ID_VISTA_FULLSCREEN, MF_CHECKED);
			else pSubMenu->CheckMenuItem(ID_VISTA_FULLSCREEN, MF_UNCHECKED);
			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				point.x, point.y, this);
		}
	}
}


// OnMouseMove: Funci� que es crida quan es mou el mouse. La utilitzem per la 
//				  Visualitzaci� Interactiva amb les tecles del mouse apretades per 
//				  modificar els par�metres de P.V. (R,angleh,anglev) segons els 
//				  moviments del mouse.
//      PARAMETRES: - nFlags: Flags que controlen si el bot� es apretat o no.
//					- point: Estructura (x,y) que d�na la posici� del mouse 
//							 (coord. pantalla) quan el bot� s'ha apretat.
void CEntornVGIView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Agregue aqu� su c�digo de controlador de mensajes o llame al valor predeterminado
	double modul = 0;
	GLdouble vdir[3] = { 0, 0, 0 };

	// TODO: Add your message handler code here and/or call default
	if (m_ButoEAvall && mobil && projeccio != CAP)
	{
		// Entorn VGI: Determinaci� dels angles (en graus) segons l'increment
		//				horitzontal i vertical de la posici� del mouse per c�meres Esf�rica i Geode.
		CSize gir = m_PosEAvall - point;
		CPoint posEAvallOld = m_PosEAvall;
		m_PosEAvall = point;
		if (camera == CAM_ESFERICA)
		{	// C�mera Esf�rica
			OPV.beta = OPV.beta + gir.cx / 2;
			//OPV.alfa = OPV.alfa + gir.cy / 2;

			// Entorn VGI: Control per evitar el creixement desmesurat dels angles.
			if (OPV.alfa >= 360)	OPV.alfa = OPV.alfa - 360;
			if (OPV.alfa < 0)			OPV.alfa = OPV.alfa + 360;
			if (OPV.beta >= 360)	OPV.beta = OPV.beta - 360;
			if (OPV.beta < 0)			OPV.beta = OPV.beta + 360;


			// zoom amb el bot� esquerra
			CSize zoomincr = posEAvallOld - point;
			long int incr = zoomincr.cy / 4.0;
			OPV.R = OPV.R + incr;
			if (OPV.R < 0.25) OPV.R = 0.25;
		}
		else { // C�mera Geode
			OPV_G.beta = OPV_G.beta + gir.cx / 2;
			OPV_G.alfa = OPV_G.alfa + gir.cy / 2;
			// Entorn VGI: Control per evitar el creixement desmesurat dels angles
			if (OPV_G.alfa >= 360.0f)	OPV_G.alfa = OPV_G.alfa - 360;
			if (OPV_G.alfa < 0.0f)		OPV_G.alfa = OPV_G.alfa + 360;
			if (OPV_G.beta >= 360.f)	OPV_G.beta = OPV_G.beta - 360;
			if (OPV_G.beta < 0.0f)		OPV_G.beta = OPV_G.beta + 360;
		}
		InvalidateRect(NULL, false);
	}
	else if (m_ButoEAvall && camera == CAM_NAVEGA && (projeccio != CAP && projeccio != ORTO)) // Opci� Navegaci�
	{
		// Entorn VGI: Canviar orientaci� en opci� de Navegaci�
		CSize girn = m_PosEAvall - point;
		angleZ = girn.cx / 2.0;
		// Entorn VGI: Control per evitar el creixement desmesurat dels angles.
		if (angleZ >= 360) angleZ = angleZ - 360;
		if (angleZ < 0)	angleZ = angleZ + 360;

		n[0] = n[0] - opvN.x;
		n[1] = n[1] - opvN.y;
		n[0] = n[0] * cos(angleZ * PI / 180) - n[1] * sin(angleZ * PI / 180);
		n[1] = n[0] * sin(angleZ * PI / 180) + n[1] * cos(angleZ * PI / 180);
		n[0] = n[0] + opvN.x;
		n[1] = n[1] + opvN.y;

		m_PosEAvall = point;
		InvalidateRect(NULL, false);
	}

	// Entorn VGI: Transformaci� Geom�trica interactiva pels eixos X,Y boto esquerra del mouse.
	else {
		bool transE = transX || transY;
		if (m_ButoEAvall && transE && transf)
		{
			// Calcular increment
			CSize girT = m_PosEAvall - point;
			if (transX)
			{
				long int incrT = girT.cx;
				if (trasl)
				{
					TG.VTras.x += incrT * fact_Tras;
					if (TG.VTras.x < -100000) TG.VTras.x = 100000;
					if (TG.VTras.x > 100000) TG.VTras.x = 100000;
				}
				else if (rota)
				{
					TG.VRota.x += incrT * fact_Rota;
					while (TG.VRota.x >= 360) TG.VRota.x -= 360;
					while (TG.VRota.x < 0) TG.VRota.x += 360;
				}
				else if (escal)
				{
					if (incrT < 0) incrT = -1 / incrT;
					TG.VScal.x = TG.VScal.x * incrT;
					if (TG.VScal.x < 0.25) TG.VScal.x = 0.25;
					if (TG.VScal.x > 8192) TG.VScal.x = 8192;
				}
			}
			if (transY)
			{
				long int incrT = girT.cy;
				if (trasl)
				{
					TG.VTras.y += incrT * fact_Tras;
					if (TG.VTras.y < -100000) TG.VTras.y = 100000;
					if (TG.VTras.y > 100000) TG.VTras.y = 100000;
				}
				else if (rota)
				{
					TG.VRota.y += incrT * fact_Rota;
					while (TG.VRota.y >= 360) TG.VRota.y -= 360;
					while (TG.VRota.y < 0) TG.VRota.y += 360;
				}
				else if (escal)
				{
					if (incrT <= 0) {
						if (incrT >= -2) incrT = -2;
						incrT = 1 / Log2(-incrT);
					}
					else incrT = Log2(incrT);
					TG.VScal.y = TG.VScal.y * incrT;
					if (TG.VScal.y < 0.25) TG.VScal.y = 0.25;
					if (TG.VScal.y > 8192) TG.VScal.y = 8192;
				}
			}
			m_PosEAvall = point;
			InvalidateRect(NULL, false);
		}
	}

	// Entorn VGI: Determinaci� del despla�ament del pan segons l'increment
	//				vertical de la posici� del mouse (tecla dreta apretada).
	if (m_ButoDAvall && pan && (projeccio != CAP && projeccio != ORTO))
	{
		CSize zoomincr = m_PosDAvall - point;
		long int incrx = zoomincr.cx;
		long int incry = zoomincr.cy;

		// Despla�ament pan vertical
		tr_cpv.y -= incry * fact_pan;
		if (tr_cpv.y > 100000) tr_cpv.y = 100000;
		else if (tr_cpv.y < -100000) tr_cpv.y = -100000;

		// Despla�ament pan horitzontal
		tr_cpv.x += incrx * fact_pan;
		if (tr_cpv.x > 100000) tr_cpv.x = 100000;
		else if (tr_cpv.x < -100000) tr_cpv.x = -100000;

		m_PosDAvall = point;
		InvalidateRect(NULL, false);
	}
	// Determinaci� del par�metre R segons l'increment
	//   vertical de la posici� del mouse (tecla dreta apretada)
		//else if (m_ButoDAvall && zzoom && (projeccio!=CAP && projeccio!=ORTO))
	else if (m_ButoDAvall && (zzoom || zzoomO) && (projeccio != CAP))
	{
		CSize zoomincr = m_PosDAvall - point;
		long int incr = zoomincr.cy / 1.0;
		if (camera == CAM_ESFERICA) {	// C�mera Esf�rica
			OPV.R = OPV.R + incr;
			if (OPV.R < 0.25) OPV.R = 0.25;
		}
		else { // C�mera Geode
			OPV_G.R = OPV_G.R + incr;
			if (OPV_G.R < 0.0f) OPV_G.R = 0.0f;
		}
		m_PosDAvall = point;
		InvalidateRect(NULL, false);
	}
	else if (m_ButoDAvall && camera == CAM_NAVEGA && (projeccio != CAP && projeccio != ORTO))
	{	// Avan�ar en opci� de Navegaci�
		if (m_PosDAvall != point)
		{
			CSize zoomincr = m_PosDAvall - point;

			double incr = zoomincr.cy / 2;
			//			long int incr=zoomincr.cy/2.0;  // Causa assertion en "afx.inl" lin 169
			vdir[0] = n[0] - opvN.x;
			vdir[1] = n[1] - opvN.y;
			vdir[2] = n[2] - opvN.z;
			modul = sqrt(vdir[0] * vdir[0] + vdir[1] * vdir[1] + vdir[2] * vdir[2]);
			vdir[0] = vdir[0] / modul;
			vdir[1] = vdir[1] / modul;
			vdir[2] = vdir[2] / modul;
			opvN.x += incr * vdir[0];
			opvN.y += incr * vdir[1];
			n[0] += incr * vdir[0];
			n[1] += incr * vdir[1];
			m_PosDAvall = point;
			InvalidateRect(NULL, false);
		}
	}

	// Entorn VGI: Transformaci� Geom�trica interactiva per l'eix Z amb boto dret del mouse.
	else if (m_ButoDAvall && transZ && transf)
	{
		// Calcular increment
		CSize girT = m_PosDAvall - point;
		long int incrT = girT.cy;
		if (trasl)
		{
			TG.VTras.z += incrT * fact_Tras;
			if (TG.VTras.z < -100000) TG.VTras.z = 100000;
			if (TG.VTras.z > 100000) TG.VTras.z = 100000;
		}
		else if (rota)
		{
			incrT = girT.cx;
			TG.VRota.z += incrT * fact_Rota;
			while (TG.VRota.z >= 360) TG.VRota.z -= 360;
			while (TG.VRota.z < 0) TG.VRota.z += 360;
		}
		else if (escal)
		{
			if (incrT <= 0) {
				if (incrT >= -2) incrT = -2;
				incrT = 1 / Log2(-incrT);
			}
			else incrT = Log2(incrT);
			TG.VScal.z = TG.VScal.z * incrT;
			if (TG.VScal.z < 0.25) TG.VScal.z = 0.25;
			if (TG.VScal.z > 8192) TG.VScal.z = 8192;
		}

		m_PosDAvall = point;
		InvalidateRect(NULL, false);
	}

	// Do not call CView::OnPaint() for painting messages
	CView::OnMouseMove(nFlags, point);
}

// OnMouseWheel: Funci� que es crida quan es mou el rodet del mouse. La utilitzem per la 
//				  Visualitzaci� Interactiva per modificar el par�metre R de P.V. (R,angleh,anglev) 
//				  segons el moviment del rodet del mouse.
//      PARAMETRES: - nFlags: Flags que controlen si el bot� es apretat o no.
//					- zDelta: Unitats de despla�ament del rodet del mouse.
//					- pt: Estructura (x,y) que d�na la posici� del mouse 
//							 (coord. pantalla) quan el bot� s'ha apretat.
BOOL CEntornVGIView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Agregue aqu� su c�digo de controlador de mensajes o llame al valor predeterminado
	double modul = 0;
	GLdouble vdir[3] = { 0, 0, 0 };

	// Funci� de zoom quan est� activada la funci� pan o les T. Geom�triques
	if ((zzoom || zzoomO) || (transX) || (transY) || (transZ))
	{
		if (camera == CAM_ESFERICA) {	// C�mera Esf�rica
			OPV.R = OPV.R + zDelta / 4;
			if (OPV.R < 0.25) OPV.R = 0.25;
			InvalidateRect(NULL, false);
		}
		else if (camera == CAM_GEODE)
		{	// C�mera Geode
			OPV_G.R = OPV_G.R + zDelta / 4.0f;
			if (OPV_G.R < 0.0f) OPV_G.R = 0.0f;
			InvalidateRect(NULL, false);
		}
	}
	else if (camera == CAM_NAVEGA)
	{
		vdir[0] = n[0] - opvN.x;
		vdir[1] = n[1] - opvN.y;
		vdir[2] = n[2] - opvN.z;
		modul = sqrt(vdir[0] * vdir[0] + vdir[1] * vdir[1] + vdir[2] * vdir[2]);
		vdir[0] = vdir[0] / modul;
		vdir[1] = vdir[1] / modul;
		vdir[2] = vdir[2] / modul;
		opvN.x += (zDelta / 4) * vdir[0];
		opvN.y += (zDelta / 4) * vdir[1];
		n[0] += (zDelta / 4) * vdir[0];
		n[1] += (zDelta / 4) * vdir[1];
		InvalidateRect(NULL, false);
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


/* ------------------------------------------------------------------------- */
/*					     TIMER (ANIMACI�)									 */
/* ------------------------------------------------------------------------- */
void CEntornVGIView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Agregue aqu� su c�digo de controlador de mensajes o llame al valor predeterminado
	if (anima) {
		// Codi de tractament de l'animaci� quan transcorren els ms. del crono.

		// Crida a OnPaint() per redibuixar l'escena
		InvalidateRect(NULL, false);
	}
	else if (satelit) {	// OPCI� SAT�LIT: Increment OPV segons moviments mouse.
		//OPV.R = OPV.R + m_EsfeIncEAvall.R;
		OPV.alfa = OPV.alfa + m_EsfeIncEAvall.alfa;
		while (OPV.alfa > 360) OPV.alfa = OPV.alfa - 360;	while (OPV.alfa < 0) OPV.alfa = OPV.alfa + 360;
		OPV.beta = OPV.beta + m_EsfeIncEAvall.beta;
		while (OPV.beta > 360) OPV.beta = OPV.beta - 360;	while (OPV.beta < 0) OPV.beta = OPV.beta + 360;

		// Crida a OnPaint() per redibuixar l'escena
		InvalidateRect(NULL, false);
	}
	if (tetris) {
		if (pieces[numPiece].cauPeca(m_board.m_blocks)) {
			pieces[numPiece].ViewcauPeca();
		}
		else {
			numPiece = rand() % (sizeof(pieces) / sizeof(pieces[0]));
			pieces[numPiece].posIni();
		}

		InvalidateRect(NULL, false);
	}

	CView::OnTimer(nIDEvent);
}


/* ------------------------------------------------------------------------- */
/*   RECURSOS DE MENU (persianes) DE L'APLICACIO:                            */
/*					1. ARXIUS												 */
/*					4. C�MERA: Esf�rica (Mobil, Zoom, ZoomO, Satelit), Navega*/
/*					5. VISTA: Pan, Eixos i Grid							     */
/*					6. PROJECCI�                                             */
/*					7. OBJECTE					                             */
/*					8. TRANSFORMA											 */
/*					9. OCULTACIONS											 */
/*				   10. IL.LUMINACI�											 */
/*				   11. LLUMS												 */
/*				   12. SHADERS												 */
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/*					1. ARXIUS 												 */
/* ------------------------------------------------------------------------- */

// Obrir fitxer Fractal
void CEntornVGIView::OnArxiuObrirFractal()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	// Entorn VGI: Obrir di�leg de lectura de fitxer (fitxers (*.MNT)
	CFileDialog openMunt(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("MNT Files(*.mnt)|*.mnt|Error Files (*.err)|*err|All Files (*.*)|*.*||"));;

	if (openMunt.DoModal() != IDOK)
		return;                 // stay with old data file
	else nom = openMunt.GetPathName();

	// Entorn VGI: Variable de tipus CString 'nom' cont� el nom del fitxer seleccionat

	// Entorn VGI: Conversi� de la variable CString nom a la variable char *nomfitx, 
	//		compatible amb  les funcions de c�rrega de fitxers fractals
	char* nomfitx = CString2Char(nom);

	// Entorn VGI: Variable de tipus char *nomfitx cont� el nom del fitxer seleccionat


	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


// OnArchivoObrirFitxerObj: Obrir fitxer en format gr�fic OBJ
void CEntornVGIView::OnArxiuObrirFitxerObj()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	//if (ObOBJ != NULL) delete ObOBJ;

	objecte = OBJOBJ;	textura = true;		tFlag_invert_Y = false;

	// Entorn VGI: Obrir di�leg de lectura de fitxer
	CFileDialog openOBJ(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("OBJ Files(*.obj)|*.obj|Error Files (*.err)|*err|All Files (*.*)|*.*||"));;

	if (openOBJ.DoModal() != IDOK)	return;  // stay with old data file
	else nom = openOBJ.GetPathName();

	// Entorn VGI: Variable de tipus CString 'nom' cont� el nom del fitxer seleccionat

	// Entorn VGI: Conversi� de la variable CString nom a la variable char *nomfitx, 
	//		compatible amb  les funcions de c�rrega de fitxers fractals
	char* nomfitx = CString2Char(nom);

	// i carreguem
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);	// Activem contexte OpenGL

	if (ObOBJ == NULL) ObOBJ = new COBJModel;
	if (vaoId_OBJ != 0) deleteVAOList(FIT_OBJ); // Eliminar VAO anterior.
	vaoId_OBJ = ObOBJ->LoadModel(nomfitx, FIT_OBJ);	// Carregar objecte OBJ AMB textura

//	Pas de par�metres textura al shader
	if (shader_menu != CAP_SHADER) glUniform1i(glGetUniformLocation(shader_programID, "textur"), textura);
	if (shader_menu != CAP_SHADER) glUniform1i(glGetUniformLocation(shader_programID, "flag_invert_y"), tFlag_invert_Y);

	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);	// Desactivem contexte OpenGL

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);

}

// Obrir fitxer en format gr�fic 3DS
void CEntornVGIView::OnArxiuObrirFitxer3ds()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
		//if(Ob3DS!=NULL) delete Ob3DS;

	objecte = OBJ3DS;	textura = true;		tFlag_invert_Y = false;

	// Entorn VGI: Obrir di�leg de lectura de fitxer
	CFileDialog open3DS(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("3DS Files(*.3ds)|*.3ds|Error Files (*.err)|*err|All Files (*.*)|*.*||"));;

	if (open3DS.DoModal() != IDOK)	return;  // stay with old data file
	else nom = open3DS.GetPathName();

	// Entorn VGI: Variable de tipus CString 'nom' cont� el nom del fitxer seleccionat

	// Entorn VGI: Conversi� de la variable CString nom a la variable char *nomfitx, 
	//		compatible amb  les funcions de c�rrega de fitxers fractals
	char* nomfitx = CString2Char(nom);

	// i carreguem
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC); // Activem contexte OpenGL

	if (Ob3DS == NULL) Ob3DS = new Obj_3DS;
	Ob3DS->EliminarMemoria();
	Ob3DS->Inicialitzar();
	Ob3DS->Carregar3DS(nomfitx);

	// Precompilaci� de dos objectes nous: OBJECTE3DS: Objecte 3DS sense textures i OBJECTE3DST amb textures,
	vaoId_3DS = Get_VAOId(FIT_3DS);
	if (vaoId_3DS != 0) deleteVAOList(FIT_3DS); // Eliminar VAO anterior.
	Ob3DS->Dibuixa3DS(false, FIT_3DS);

	//	Pas de par�metres textura al shader
	if (shader_menu != CAP_SHADER) glUniform1i(glGetUniformLocation(shader_programID, "textur"), textura);
	if (shader_menu != CAP_SHADER) glUniform1i(glGetUniformLocation(shader_programID, "flag_invert_y"), tFlag_invert_Y);

	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL); // Desactivem contexte OpenGL

//  Entorn VGI: Modificar R per centrar Vista amb mida de l'objecte
	mida = sqrtf(3.0f) * 10.0;
	OPV.R = 0.5 * mida / sin(30 * PI / 180) + p_near;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);

}

/* ------------------------------------------------------------------------- */
/*					4. C�MERA: Esf�rica (Mobil, Zoom, ZoomO, Satelit), Navega*/
/* ------------------------------------------------------------------------- */
// C�MERA: Mode Esf�rica (C�mera esf�rica en polars-opci� booleana)
void CEntornVGIView::OnCameraEsferica()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	camera = CAM_ESFERICA;

	// Inicialitzar par�metres C�mera Esf�rica
	OPV.R = 15.0;		OPV.alfa = 0.0;		OPV.beta = 0.0;				// Origen PV en esf�riques
	mobil = true;	zzoom = true;	satelit = false;
	Vis_Polar = POLARZ;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateCameraEsferica(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (camera == CAM_ESFERICA) 	pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// C�MERA--> ESFERICA: Mobil. Punt de Vista Interactiu (opci� booleana)
void CEntornVGIView::OnVistaMobil()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	if ((projeccio != ORTO) || (projeccio != CAP) && (camera == CAM_ESFERICA || camera == CAM_GEODE))  mobil = !mobil;
	// Desactivaci� de Transformacions Geom�triques via mouse 
	//		si Visualitzaci� Interactiva activada.	
	if (mobil) {
		transX = false;	transY = false; transZ = false;
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateVistaMobil(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if ((camera == CAM_ESFERICA || camera == CAM_GEODE) && (mobil)) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

// C�MERA--> ESFERICA: Zoom. Zoom Interactiu (opci� booleana)
void CEntornVGIView::OnVistaZoom()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	if ((projeccio != ORTO) || (projeccio != CAP) && (camera == CAM_ESFERICA || camera == CAM_GEODE)) zzoom = !zzoom;
	// Desactivaci� de Transformacions Geom�triques via mouse 
	//		si Zoom activat.
	if (zzoom) {
		transX = false;	transY = false;	transZ = false;
		zzoomO = false;
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateVistaZoom(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if ((camera == CAM_ESFERICA || camera == CAM_GEODE) && zzoom) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// C�MERA--> ESFERICA: Zoom Orto. Zoom Interactiu en Ortogr�fica (opci� booleana)
void CEntornVGIView::OnVistaZoomOrto()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	if (projeccio != CAP || projeccio != PERSPECT) zzoomO = !zzoomO;
	// Desactivaci� de Transformacions Geom�triques via mouse 
	//	si Zoom activat
	if (zzoomO) {
		zzoom = false;
		transX = false;	transY = false;	transZ = false;
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateVistaZoomOrto(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if ((camera == CAM_ESFERICA || camera == CAM_GEODE) && zzoomO) pCmdUI->SetCheck(1);
}


// C�MERA--> ESFERICA: Sat�lit. Vista interactiva i animada en que increment de movimen �s activat per mouse (opci� booleana)
void CEntornVGIView::OnVistaSatelit()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	if ((projeccio != CAP || projeccio != ORTO) && (camera == CAM_ESFERICA)) satelit = !satelit;
	if (satelit) mobil = true;
	bool testA = anima;									// Testejar si hi ha alguna animaci� activa apart de Sat�lit.
	if ((!satelit) && (!testA)) KillTimer(WM_TIMER);	// Si es desactiva Sat�lit i no hi ha cap animaci� activa es desactiva el Timer.

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateVistaSatelit(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if ((camera == CAM_ESFERICA) && (satelit)) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// C�MERA--> ESFERICA: Polars Eix X cap amunt per a Visualitzaci� Interactiva
void CEntornVGIView::OnVistaPolarsX()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	if ((projeccio != CAP) && (camera == CAM_ESFERICA)) Vis_Polar = POLARX;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateVistaPolarsX(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if ((camera == CAM_ESFERICA) && (Vis_Polar == POLARX)) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// C�MERA--> ESFERICA: Polars Eix Y cap amunt per a Visualitzaci� Interactiva
void CEntornVGIView::OnVistaPolarsY()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	if ((projeccio != CAP) && (camera == CAM_ESFERICA)) Vis_Polar = POLARY;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateVistaPolarsY(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if ((camera == CAM_ESFERICA) && (Vis_Polar == POLARY)) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// C�MERA--> ESFERICA: Polars Eix Z cap amunt per a Visualitzaci� Interactiva
void CEntornVGIView::OnVistaPolarsZ()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	if ((projeccio != CAP) && (camera == CAM_ESFERICA)) Vis_Polar = POLARZ;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateVistaPolarsZ(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if ((camera == CAM_ESFERICA) && (Vis_Polar == POLARZ)) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// C�MERA--> NAVEGA:  Mode de navegaci� sobre un pla amb botons de teclat o de mouse (nav) (opci� booleana)
void CEntornVGIView::OnVistaNavega()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	if (projeccio != ORTO) camera = CAM_NAVEGA;

	// Desactivaci� de zoom, mobil, Transformacions Geom�triques via mouse i pan
	mobil = false;	zzoom = false;
	transX = false;	transY = false;	transZ = false;
	//pan = false;
	tr_cpv.x = 0.0;		tr_cpv.y = 0.0;		tr_cpv.z = 0.0;		// Inicialitzar a 0 despla�ament de pantalla
	tr_cpvF.x = 0.0;	tr_cpvF.y = 0.0;	tr_cpvF.x = 0.0;	// Inicialitzar a 0 despla�ament de pantalla

// Incialitzar variables Navega
	n[0] = 0.0;		n[1] = 0.0;		n[2] = 0.0;
	opvN.x = 10.0;	opvN.y = 0.0;		opvN.z = 0.0;
	angleZ = 0.0;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateVistaNavega(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (camera == CAM_NAVEGA) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

// Tornar a lloc d'origen
void CEntornVGIView::OnVistaOrigennavega()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	if (camera == CAM_NAVEGA) {
		n[0] = 0.0;		n[1] = 0.0;		n[2] = 0.0;
		opvN.x = 10.0;	opvN.y = 0.0;		opvN.z = 0.0;
		angleZ = 0.0;
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}



// C�MERA--> GEODE:  Mode de navegaci� centrat a l'origent mirant un punt en coord. esf�riques (R,alfa,beta) (opci� booleana)
void CEntornVGIView::OnCameraGeode()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	camera = CAM_GEODE;

	// Inicialitzar par�metres C�mera Geode
	OPV.R = 0.0;		OPV.alfa = 0.0;		OPV.beta = 0.0;				// Origen PV en esf�riques
	mobil = true;	zzoom = true;	zzoomO = false;	 satelit = false;	pan = false;
	Vis_Polar = POLARZ;
	llumGL[5].encesa = true;

	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);	// Activem contexte OpenGL
	glFrontFace(GL_CW);
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL); // Desactivem contexte OpenGL

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);

}


void CEntornVGIView::OnUpdateCameraGeode(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (camera == CAM_GEODE) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


void CEntornVGIView::OnCameraOrigenGeode()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	// Inicialitzar par�metres C�mera Geode
	OPV.R = 0.0;		OPV.alfa = 0.0;		OPV.beta = 0.0;				// Origen PV en esf�riques
	mobil = true;	zzoom = true;	zzoomO = false;		 satelit = false;	pan = false;
	Vis_Polar = POLARZ;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


/* -------------------------------------------------------------------------------- */
/*					5. VISTA: Pantalla Completa, Pan i Eixos	                    */
/* -------------------------------------------------------------------------------- */
// VISTA: FullScreen (Pantalla Completa-opci� booleana)
void CEntornVGIView::OnVistaFullscreen()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos

	if (!fullscreen)
	{	// I note that I go to full-screen mode
		fullscreen = true;
		// Remembers the address of the window in which the view was placed (probably a frame)
		saveParent = this->GetParent();
		// Assigns a view to a new parent - desktop
		this->SetParent(GetDesktopWindow());
		CRect rect; // it's about the dimensions of the desktop-desktop
		GetDesktopWindow()->GetWindowRect(&rect);
		// I set the window on the desktop
		MoveWindow(rect);
	}
	else {	// Switching off the full-screen mode
		fullscreen = false;
		// Assigns an old parent view
		this->SetParent(saveParent);
		CRect rect; // It's about the dimensions of the desktop-desktop
		// Get client screen dimensions
		saveParent->GetClientRect(&rect);
		// Changes the position and dimensions of the specified window.
		MoveWindow(rect, FALSE);
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateVistaFullscreen(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (fullscreen) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// VISTA: Mode de Despla�ament horitzontal i vertical per pantalla del Punt de Vista (pan) (opci� booleana)
void CEntornVGIView::OnVistaPan()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	if ((projeccio != ORTO) || (projeccio != CAP)) pan = !pan;
	// Desactivaci� de Transformacions Geom�triques via mouse i navega si pan activat
	if (pan) {
		mobil = true;		zzoom = true;
		transX = false;		transY = false;		transZ = false;
		//navega = false;
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateVistaPan(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (pan) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

// Tornar a lloc d'origen
void CEntornVGIView::OnVistaOrigenpan()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	if (pan) {
		fact_pan = 1;
		tr_cpv.x = 0;	tr_cpv.y = 0;	tr_cpv.z = 0;
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}



// VISTA: Visualitzar eixos coordenades m�n (opci� booleana)
void CEntornVGIView::OnVistaEixos()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	eixos = !eixos;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateVistaEixos(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (eixos) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// SKYBOX: Visualitzar Skybox en l'escena (opci� booleana)
void CEntornVGIView::OnVistaSkyBox()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	SkyBoxCube = !SkyBoxCube;

	// Entorn VGI: Activaci� el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	// C�rrega Shader Skybox
	if (!skC_programID) skC_programID = shader_SkyBoxC.loadFileShaders(".\\shaders\\skybox.VERT", ".\\shaders\\skybox.FRAG");

	// C�rrega VAO Skybox Cube
	if (skC_VAOID.vaoId == -1) skC_VAOID = loadCubeSkybox_VAO();
	Set_VAOList(CUBE_SKYBOX, skC_VAOID);

	if (!cubemapTexture)
	{	// load Skybox textures
		// -------------
		std::vector<std::string> faces =
		{ ".\\textures\\skybox\\right.jpg",
			".\\textures\\skybox\\left.jpg",
			".\\textures\\skybox\\top.jpg",
			".\\textures\\skybox\\bottom.jpg",
			".\\textures\\skybox\\front.jpg",
			".\\textures\\skybox\\back.jpg"
		};
		cubemapTexture = loadCubemap(faces);
	}

	// Entorn VGI: Activaci� el contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateVistaSkyBox(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (SkyBoxCube) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// VISTA: Visualitzar Grid XY (opci� booleana)
void CEntornVGIView::OnVistaGridXY()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	grid.x = !grid.x;	hgrid.x = 0.0;
	if (grid.x) grid.w = false;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateVistaGridXY(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (grid.x) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

// VISTA: Visualitzar Grid XZ (opci� booleana)
void CEntornVGIView::OnVistaGridXZ()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	grid.y = !grid.y;	hgrid.y = 0.0;
	if (grid.y) grid.w = false;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateVistaGridXZ(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (grid.y) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// VISTA: Visualitzar Grid YZ (opci� booleana)
void CEntornVGIView::OnVistaGridYZ()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	grid.z = !grid.z;	hgrid.z = 0.0;
	if (grid.z) grid.w = false;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateVistaGridYZ(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (grid.z) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// VISTA: Visualitzar Grid XYZ (opci� booleana)
void CEntornVGIView::OnVistaGridXYZ()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	grid.w = !grid.w;	hgrid.w = 0.0;
	if (grid.w)
	{
		grid.x = false;	grid.y = false;		grid.z = false;
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateVistaGridXYZ(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (grid.w) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);

}


/* ------------------------------------------------------------------------- */
/*					6. PROJECCI�                                             */
/* ------------------------------------------------------------------------- */

// PROJECCI�: Perspectiva
void CEntornVGIView::OnProjeccioPerspectiva()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	projeccio = PERSPECT;
	mobil = true;			zzoom = true;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);

}

void CEntornVGIView::OnUpdateProjeccioPerspectiva(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (projeccio == PERSPECT) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


/* ------------------------------------------------------------------------- */
/*					7. OBJECTE					                             */
/* ------------------------------------------------------------------------- */

// OBJECTE: Cub
void CEntornVGIView::OnObjecteCub()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos

	objecte = CUB;
	CVAO cubeVAO;

	//  Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)
	//	Canviar l'escala per a centrar la vista (Ortogr�fica)

	// Entorn VGI: Activaci� el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	//netejaVAOList();						// Neteja Llista VAO.
	if (Get_VAOId(GLUT_CUBE) != 0) deleteVAOList(GLUT_CUBE);				// Neteja VAO Cub
	cubeVAO = loadglutSolidCube_VAO(1.0);	// C�rrega cub de costat 1 com a VAO
	Set_VAOList(GLUT_CUBE, cubeVAO);		// Guarda (vaoId, vboId, nVertexs) a la posici� GLUT_CUBE.

// Entorn VGI: Activaci� el contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateObjecteCub(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (objecte == CUB) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}



// OBJECTE: Cub RGB
void CEntornVGIView::OnObjecteCubRGB()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	objecte = CUB_RGB;
	CVAO cubeVAO;

	//  Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)
	//	Canviar l'escala per a centrar la vista (Ortogr�fica)

	// Entorn VGI: Activaci� el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	//netejaVAOList();						// Neteja Llista VAO.
	if (Get_VAOId(GLUT_CUBE_RGB) != 0) deleteVAOList(GLUT_CUBE_RGB);
	cubeVAO = loadglutSolidCubeRGB_VAO(1.0);	// C�rrega cub RGB de costat 1 com a VAO
	Set_VAOList(GLUT_CUBE_RGB, cubeVAO);		// Guarda (vaoId, vboId, nVertexs) a la posici� GLUT_CUBE_RGB.

// Entorn VGI: Activaci� el contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateObjecteCubRGB(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (objecte == CUB_RGB) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE Esfera
void CEntornVGIView::OnObjecteEsfera()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	objecte = ESFERA;

	//  Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)
	//	Canviar l'escala per a centrar la vista (Ortogr�fica)

	// Entorn VGI: Activaci� el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	//	netejaVAOList();						// Neteja Llista VAO.
	if (Get_VAOId(GLU_SPHERE) != 0) deleteVAOList(GLU_SPHERE);
	CVAO sphereVAO = loadgluSphere_VAO(1.0, 20, 20);	// C�rrega cub RGB de costat 1 com a VAO
	Set_VAOList(GLU_SPHERE, sphereVAO);

	// Entorn VGI: Activaci� el contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateObjecteEsfera(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (objecte == ESFERA) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

// OBJECTE Tetera
void CEntornVGIView::OnObjecteTetera()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	objecte = TETERA;

	//  Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)
	//	Canviar l'escala per a centrar la vista (Ortogr�fica)

	// Entorn VGI: Activaci� el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	//	netejaVAOList();						// Neteja Llista VAO.
	if (Get_VAOId(GLUT_TEAPOT) != 0) deleteVAOList(GLUT_TEAPOT);
	CVAO teteraVAO = loadglutSolidTeapot_VAO(1.0);	// C�rrega Teapot de costat 1 com a VAO
	Set_VAOList(GLUT_TEAPOT, teteraVAO);

	// Entorn VGI: Activaci� el contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateObjecteTetera(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (objecte == TETERA) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// --------------- OBJECTES COMPLEXES

// OBJECTE ARC
void CEntornVGIView::OnObjecteArc()
{
	CColor color_Mar;
	CVAO objectVAO;

	color_Mar.r = 0.5;	color_Mar.g = 0.4; color_Mar.b = 0.9; color_Mar.a = 1.0;
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	objecte = ARC;

	//  Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)
	//	Canviar l'escala per a centrar la vista (Ortogr�fica)

	// Entorn VGI: Activaci� el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	// C�rrega dels VAO's per a construir objecte ARC
	//	netejaVAOList();						// Neteja Llista VAO.
	if (Get_VAOId(GLUT_CUBE) != 0) deleteVAOList(GLUT_CUBE);
	objectVAO = loadglutSolidCube_VAO(1.0);		// C�rrega Cub de costat 1 com a VAO
	Set_VAOList(GLUT_CUBE, objectVAO);			// Guarda (vaoId, vboId, nVertexs) a la posici� GLUT_CUBE.

	if (Get_VAOId(GLU_SPHERE) != 0) deleteVAOList(GLU_SPHERE);
	objectVAO = loadgluSphere_VAO(0.5, 20, 20);
	Set_VAOList(GLU_SPHERE, objectVAO);			// Guarda (vaoId, vboId, nVertexs) a la posici� GLU_SPHERE.

	if (Get_VAOId(GLUT_TEAPOT) != 0) deleteVAOList(GLUT_TEAPOT);
	objectVAO = loadglutSolidTeapot_VAO(1.0);
	Set_VAOList(GLUT_TEAPOT, objectVAO);			// Guarda (vaoId, vboId, nVertexs) a la posici� GLUT_TEAPOT.

	if (Get_VAOId(MAR_FRACTAL_VAO) != 0) deleteVAOList(MAR_FRACTAL_VAO);
	objectVAO = loadSea_VAO(color_Mar);
	Set_VAOList(MAR_FRACTAL_VAO, objectVAO);			// Guarda (vaoId, vboId, nVertexs) a la posici� MAR_FRACTAL_VAO.

// Entorn VGI: Activaci� el contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateObjecteArc(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (objecte == ARC) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE Tie
void CEntornVGIView::OnObjecteTie()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	objecte = TIE;
	CVAO objectVAO;

	//  Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)
	//	Canviar l'escala per a centrar la vista (Ortogr�fica)

	// Entorn VGI: Activaci� el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	// C�rrega dels VAO's per a construir objecte ARC
		//	netejaVAOList();						// Neteja Llista VAO.
	if (Get_VAOId(GLU_CYLINDER) != 0) deleteVAOList(GLU_CYLINDER);
	objectVAO = loadgluCylinder_VAO(5.0f, 5.0f, 0.5f, 6, 1);	// C�rrega cilindre com a VAO
	//Set_VAOList(GLUT_CYLINDER, objectVAO);			// Guarda (vaoId, vboId, nVertexs) a la posici� GLUT_CYLINDER.

	if (Get_VAOId(GLU_DISK) != 0)deleteVAOList(GLU_DISK);
	objectVAO = loadgluDisk_VAO(0.0f, 5.0f, 6, 1);
	//Set_VAOList(GLU_DISK, objectVAO);			// Guarda (vaoId, vboId, nVertexs) a la posici� GLU_DISK.

	if (Get_VAOId(GLUT_USER1) != 0)deleteVAOList(GLUT_USER1);
	objectVAO = loadgluCylinder_VAO(5.0f, 5.0f, 2.0f, 6, 1);	// C�rrega cilindre com a VAO
	Set_VAOList(GLUT_USER1, objectVAO);			// Guarda (vaoId, vboId, nVertexs) a la posici� GLUT_USER1.

	if (Get_VAOId(GLUT_CUBE) != 0)deleteVAOList(GLUT_CUBE);
	objectVAO = loadglutSolidCube_VAO(1.0);
	Set_VAOList(GLUT_CUBE, objectVAO);			// Guarda (vaoId, vboId, nVertexs) a la posici� GLUT_CUBE.

	if (Get_VAOId(GLUT_TORUS) != 0)deleteVAOList(GLUT_TORUS);
	objectVAO = loadglutSolidTorus_VAO(1.0, 5.0, 20, 20);
	Set_VAOList(GLUT_TORUS, objectVAO);			// Guarda (vaoId, vboId, nVertexs) a la posici� GLUT_TORUS.

	if (Get_VAOId(GLUT_USER2) != 0)deleteVAOList(GLUT_USER2);
	objectVAO = loadgluCylinder_VAO(1.0f, 0.5f, 5.0f, 60, 1);	// C�rrega cilindre com a VAO
	Set_VAOList(GLUT_USER2, objectVAO);			// Guarda (vaoId, vboId, nVertexs) a la posici� GLUT_USER2.

	if (Get_VAOId(GLUT_USER3) != 0)deleteVAOList(GLUT_USER3);
	objectVAO = loadgluCylinder_VAO(0.35f, 0.35f, 5.0f, 80, 1);	// C�rrega cilindre com a VAO
	Set_VAOList(GLUT_USER3, objectVAO);			// Guarda (vaoId, vboId, nVertexs) a la posici� GLUT_USER3.

	if (Get_VAOId(GLUT_USER4) != 0)deleteVAOList(GLUT_USER4);
	objectVAO = loadgluCylinder_VAO(4.0f, 2.0f, 10.25f, 40, 1);	// C�rrega cilindre com a VAO
	Set_VAOList(GLUT_USER4, objectVAO);			// Guarda (vaoId, vboId, nVertexs) a la posici� GLUT_USER4.

// Entorn VGI: Activaci� el contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateObjecteTie(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (objecte == TIE) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

// ----------------- OBJECTES CORBES BEZIER, LEMNISCATA i B-SPLINE


// OBJECTE Corba Bezier
void CEntornVGIView::OnObjeteCorbaBezier()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	CVAO objectVAO;

	nom = "";
	objecte = C_BEZIER;		sw_material[4] = true;

	// Entorn VGI: Obrir di�leg de lectura de fitxer (fitxers (*.crv)
	CFileDialog openSpline(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("SPL Files(*.crv)|*.crv|All Files (*.*)|*.*||"));;

	if (openSpline.DoModal() != IDOK)
		return;                 // stay with old data file
	else nom = openSpline.GetPathName();

	// Entorn VGI: Variable de tipus CString 'nom' cont� el nom del fitxer seleccionat

	// Entorn VGI: Conversi� de la variable CString nom a la variable char *nomfitx, 
	//		compatible amb  les funcions de c�rrega de fitxers fractals
	char* nomfitx = CString2Char(nom);

	npts_T = llegir_ptsC(nomfitx);

	// 	---- Entorn VGI: Modificar R per centrar Vista amb mida de l'objecte

	// Entorn VGI: Activaci� el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	// C�rrega dels VAO's per a construir la corba Bezier
		//	netejaVAOList();						// Neteja Llista VAO.
	deleteVAOList(GLU_SPHERE);
	objectVAO = loadgluSphere_VAO(5.0, 20, 20);
	Set_VAOList(GLU_SPHERE, objectVAO);			// Guarda (vaoId, vboId, nVertexs) a la posici� GLUT_CUBE.

	if (Get_VAOId(CRV_BEZIER) != 0) deleteVAOList(CRV_BEZIER);
	CVAO BezierVAO = load_Bezier_Curve_VAO(npts_T, PC_t, pas_CS, false);
	Set_VAOList(CRV_BEZIER, BezierVAO);

	// Entorn VGI: Activaci� el contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateObjeteCorbaBezier(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (objecte == C_BEZIER) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE Corba Lemniscata 3D
void CEntornVGIView::OnObjecteCorbaLemniscata()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	objecte = C_LEMNISCATA;		sw_material[4] = true;

	// 	---- Entorn VGI: Modificar R per centrar Vista amb mida de l'objecte

	// Entorn VGI: Activaci� el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	// C�rrega dels VAO's per a construir la corba Bezier
		//	netejaVAOList();						// Neteja Llista VAO.
	if (Get_VAOId(CRV_LEMNISCATA3D) != 0) deleteVAOList(CRV_LEMNISCATA3D);
	CVAO Lemni3DVAO = load_Lemniscata3D_VAO(800, pas_CS * 20.0);
	Set_VAOList(CRV_LEMNISCATA3D, Lemni3DVAO);

	// Entorn VGI: Activaci� el contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateObjecteCorbaLemniscata(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (objecte == C_LEMNISCATA) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE Corba B-Spline
void CEntornVGIView::OnObjecteCorbaBSpline()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	nom = "";
	objecte = C_BSPLINE;	sw_material[4] = true;

	// Entorn VGI: Obrir di�leg de lectura de fitxer (fitxers (*.crv)
	CFileDialog openSpline(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("SPL Files(*.crv)|*.crv|All Files (*.*)|*.*||"));;

	if (openSpline.DoModal() != IDOK)
		return;                 // stay with old data file
	else nom = openSpline.GetPathName();

	// Entorn VGI: Variable de tipus CString 'nom' cont� el nom del fitxer seleccionat

	// Entorn VGI: Conversi� de la variable CString nom a la variable char *nomfitx, 
	//		compatible amb  les funcions de c�rrega de fitxers fractals
	char* nomfitx = CString2Char(nom);

	npts_T = llegir_ptsC(nomfitx);

	// 	---- Entorn VGI: Modificar R per centrar Vista amb mida de l'objecte

	// Entorn VGI: Activaci� el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	// C�rrega dels VAO's per a construir la corba BSpline
			//	netejaVAOList();						// Neteja Llista VAO.
	if (Get_VAOId(GLU_SPHERE) != 0) deleteVAOList(GLU_SPHERE);
	CVAO objectVAO = loadgluSphere_VAO(5.0, 20, 20);
	Set_VAOList(GLU_SPHERE, objectVAO);			// Guarda (vaoId, vboId, nVertexs) a la posici� GLUT_CUBE.

	if (Get_VAOId(CRV_BSPLINE) != 0) deleteVAOList(CRV_BSPLINE);
	CVAO BSplineVAO = load_BSpline_Curve_VAO(npts_T, PC_t, pas_CS);
	Set_VAOList(CRV_BSPLINE, BSplineVAO);

	// Entorn VGI: Activaci� el contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);


	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateObjecteCorbaBSpline(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (objecte == C_BSPLINE) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE Punts de Control: Activaci� de la visualitzaci� dels Punts de control de les Corbes (OPCI� BOOLEANA)
void CEntornVGIView::OnObjectePuntsControl()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	sw_Punts_Control = !sw_Punts_Control;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateObjectePuntsControl(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (sw_Punts_Control) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE --> CORBES: Activar o desactivar visualitzaci� Triedre de Frenet Punts de control de les corbes 
//					Lemniscata, Bezier i BSpline
void CEntornVGIView::OnCorbesTriedreFrenet()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	dibuixa_TriedreFrenet = !dibuixa_TriedreFrenet;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateCorbesTriedreFrenet(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (dibuixa_TriedreFrenet) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE Matriu Primitives
void CEntornVGIView::OnObjecteMatriuPrimitives()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	objecte = MATRIUP;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateObjecteMatriuPrimitives(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (objecte == MATRIUP) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE Matriu Primitives VAO
void CEntornVGIView::OnObjecteMatriuPrimitivesVBO()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	// ------- VAO
	CVAO objectVAO;

	objecte = MATRIUP_VAO;

	// Entorn VGI: Activaci� el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	// C�rrega dels VAO's per a construir objecte ARC
		//	netejaVAOList();						// Neteja Llista VAO.
	if (Get_VAOId(GLUT_CUBE) != 0) deleteVAOList(GLUT_CUBE);
	objectVAO = loadglutSolidCube_VAO(1.0f);
	Set_VAOList(GLUT_CUBE, objectVAO);

	if (Get_VAOId(GLUT_TORUS) != 0)deleteVAOList(GLUT_TORUS);
	objectVAO = loadglutSolidTorus_VAO(2.0, 3.0, 20, 20);
	Set_VAOList(GLUT_TORUS, objectVAO);

	if (Get_VAOId(GLUT_SPHERE) != 0)deleteVAOList(GLU_SPHERE);
	objectVAO = loadgluSphere_VAO(1.0, 20, 20);
	Set_VAOList(GLU_SPHERE, objectVAO);

	// Entorn VGI: Activaci� el contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateObjecteMatriuPrimitivesVBO(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (objecte == MATRIUP_VAO) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


/* ------------------------------------------------------------------------- */
/*					8. TRANSFORMA											 */
/* ------------------------------------------------------------------------- */

// TRANSFORMA: TRASLACI�
void CEntornVGIView::OnTransformaTraslacio()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	trasl = !trasl;
	rota = false;
	if (trasl) escal = true;
	transf = trasl || rota || escal;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateTransformaTraslacio(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (trasl) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


void CEntornVGIView::OnTransformaOrigentraslacio()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	if (trasl)
	{
		fact_Tras = 1;
		TG.VTras.x = 0.0;	TG.VTras.y = 0.0;	TG.VTras.z = 0;
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


// TRANSFORMA: ROTACI�
void CEntornVGIView::OnTransformaRotacio()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	rota = !rota;
	trasl = false;
	if (rota) escal = true;
	transf = trasl || rota || escal;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateTransformaRotacio(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (rota) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


void CEntornVGIView::OnTransformaOrigenrotacio()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	if (rota)
	{
		fact_Rota = 90;
		TG.VRota.x = 0;		TG.VRota.y = 0;		TG.VRota.z = 0;
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


// TRANSFORMA: ESCALAT
void CEntornVGIView::OnTransformaEscalat()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	if ((!rota) && (!trasl)) escal = !escal;
	transf = trasl || rota || escal;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateTransformaEscalat(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (escal) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


void CEntornVGIView::OnTransformaOrigenescalat()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	if (escal) { TG.VScal.x = 1;	TG.VScal.y = 1;	TG.VScal.z = 1; }

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


// TRANSFOMA: M�bil Eix X? (opci� booleana).
void CEntornVGIView::OnTransformaMobilx()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	if (transf)
	{
		transX = !transX;
		if (transX) {
			mobil = false;	zzoom = false;
			pan = false;	//navega = false;
		}
		else if ((!transY) && (!transZ)) {
			mobil = true;
			zzoom = true;
		}
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateTransformaMobilx(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (transX) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// TRANSFOMA: M�bil Eix Y? (opci� booleana).
void CEntornVGIView::OnTransformaMobily()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	if (transf)
	{
		transY = !transY;
		if (transY) {
			mobil = false;	zzoom = false;
			pan = false;	//navega = false;
		}
		else if ((!transX) && (!transZ)) {
			mobil = true;
			zzoom = true;
		}
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateTransformaMobily(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (transY) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// TRANSFOMA: M�bil Eix Z? (opci� booleana).
void CEntornVGIView::OnTransformaMobilz()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	if (transf)
	{
		transZ = !transZ;
		if (transZ) {
			mobil = false;	zzoom = false;
			pan = false;	//navega = false;
		}
		else if ((!transX) && (!transY)) {
			mobil = true;
			zzoom = true;
		}
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateTransformaMobilz(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (transZ) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


/* ------------------------------------------------------------------------- */
/*					9. OCULTACIONS											 */
/* ------------------------------------------------------------------------- */

void CEntornVGIView::OnOcultacionsFrontFaces()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	front_faces = !front_faces;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateOcultacionsFrontFaces(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (front_faces) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OCULTACIONS: Test de Visibilitat? (opci� booleana).
void CEntornVGIView::OnOcultacionsTestvis()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	test_vis = !test_vis;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateOcultacionsTestvis(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (test_vis) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OCULTACIONS: Z-Buffer? (opci� booleana).
void CEntornVGIView::OnOcultacionsZbuffer()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	oculta = !oculta;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateOcultacionsZbuffer(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (oculta) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OCULTACIONS: Back Line? (opci� booleana).
void CEntornVGIView::OnOcultacionsBackline()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	back_line = !back_line;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateOcultacionsBackline(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (back_line) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


/* ------------------------------------------------------------------------- */
/*					10. IL.LUMINACI�											 */
/* ------------------------------------------------------------------------- */

// IL.LUMINACI� Font de llum fixe? (opci� booleana).
void CEntornVGIView::OnIluminacioLlumfixe()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	ifixe = !ifixe;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateIluminacioLlumfixe(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (ifixe) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// IL.LUMINACI�: Mantenir iluminades les Cares Front i Back
void CEntornVGIView::OnIluminacio2Sides()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	ilum2sides = !ilum2sides;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateIluminacio2Sides(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (ilum2sides) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}



// ILUMINACI� PUNTS
void CEntornVGIView::OnIluminacioPunts()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	ilumina = PUNTS;
	test_vis = false;		oculta = false;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateIluminacioPunts(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (ilumina == PUNTS) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACI� FILFERROS
void CEntornVGIView::OnIluminacioFilferros()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	ilumina = FILFERROS;
	test_vis = false;		oculta = false;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateIluminacioFilferros(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (ilumina == FILFERROS) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACI� PLANA
void CEntornVGIView::OnIluminacioPlana()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	ilumina = PLANA;
	test_vis = false;		oculta = true;

	// Entorn VGI: Activaci� del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	// Elimina shader anterior
	shaderLighting.DeleteProgram();
	// C�rrega Flat shader
	shader_programID = shaderLighting.loadFileShaders(".\\shaders\\flat_shdrML.vert", ".\\shaders\\flat_shdrML.frag");

	// Entorn VGI: Desactivaci� del contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);

}

void CEntornVGIView::OnUpdateIluminacioPlana(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (ilumina == PLANA) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACI� GOURAUD
void CEntornVGIView::OnIluminacioGouraud()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	ilumina = GOURAUD;
	test_vis = false;		oculta = true;

	// Entorn VGI: Activaci� del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	// Elimina shader anterior
	shaderLighting.DeleteProgram();
	// C�rrega Gouraud shader 
	shader_programID = shaderLighting.loadFileShaders(".\\shaders\\gouraud_shdrML.vert", ".\\shaders\\gouraud_shdrML.frag");

	// Entorn VGI: Desactivaci� del contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateIluminacioGouraud(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (ilumina == GOURAUD) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACI� PHONG
void CEntornVGIView::OnIluminacioPhong()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	ilumina = PHONG;
	test_vis = false;		oculta = true;

	// Entorn VGI: Activaci� del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	// Elimina shader anterior
	shaderLighting.DeleteProgram();
	// C�rrega Phong Shader
	shader_programID = shaderLighting.loadFileShaders(".\\shaders\\phong_shdrML.vert", ".\\shaders\\phong_shdrML.frag");

	// Entorn VGI: Desactivaci� del contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateIluminacioPhong(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (ilumina == PHONG) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

// ILUMINACI�->REFLECTIVITAT MATERIAL / COLOR: Activaci� i desactivaci� de la reflectivitat pr�pia del material com a color.
void CEntornVGIView::OnMaterialReflmaterial()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	sw_material[4] = !sw_material[4];

	sw_il = true;

	if (shader_menu != CAP_SHADER) {
		// Entorn VGI: Activaci� del contexte OpenGL
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		// Pas m�scara llums
		glUniform1i(glGetUniformLocation(shader_programID, "sw_material"), sw_material[4]);
		// Entorn VGI: Desactivaci� del contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateMaterialReflMaterial(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (sw_material[4])	pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACI�->REFLECTIVITAT MATERIAL EMISSI�: Activaci� i desactivaci� de la reflectivitat pr�pia del material.
void CEntornVGIView::OnMaterialEmissio()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	sw_material[0] = !sw_material[0];

	if (shader_menu != CAP_SHADER) {
		// Entorn VGI: Activaci� del contexte OpenGL
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		// Pas m�scara llums
		glUniform1i(glGetUniformLocation(shader_programID, "sw_intensity[0]"), sw_material[0]);
		// Entorn VGI: Desactivaci� del contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateMaterialEmissio(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (sw_material[0])	pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACI�->REFLECTIVITAT MATERIAL AMBIENT: Activaci� i desactivaci� de la reflectivitat ambient del material.
void CEntornVGIView::OnMaterialAmbient()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	sw_material[1] = !sw_material[1];

	if (shader_menu != CAP_SHADER) {
		// Entorn VGI: Activaci� del contexte OpenGL
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		// Pas m�scara llums
		glUniform1i(glGetUniformLocation(shader_programID, "sw_intensity[1]"), sw_material[1]);
		// Entorn VGI: Desactivaci� del contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateMaterialAmbient(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (sw_material[1])	pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACI�->REFLECTIVITAT MATERIAL DIFUSA: Activaci� i desactivaci� de la reflectivitat difusa del materials.
void CEntornVGIView::OnMaterialDifusa()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	sw_material[2] = !sw_material[2];

	if (shader_menu != CAP_SHADER) {
		// Entorn VGI: Activaci� del contexte OpenGL
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		// Pas m�scara llums
		glUniform1i(glGetUniformLocation(shader_programID, "sw_intensity[2]"), sw_material[2]);
		// Entorn VGI: Desactivaci� del contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateMaterialDifusa(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (sw_material[2])	pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACI�->REFLECTIVITAT MATERIAL ESPECULAR: Activaci� i desactivaci� de la reflectivitat especular del material.
void CEntornVGIView::OnMaterialEspecular()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	sw_material[3] = !sw_material[3];

	if (shader_menu != CAP_SHADER) {
		// Entorn VGI: Activaci� del contexte OpenGL
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		// Pas m�scara llums
		glUniform1i(glGetUniformLocation(shader_programID, "sw_intensity[3]"), sw_material[3]);
		// Entorn VGI: Desactivaci� del contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateMaterialEspecular(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (sw_material[3])	pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACI�: Textures?: Activaci� (TRUE) o desactivaci� (FALSE) de textures.
void CEntornVGIView::OnIluminacioTextures()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	textura = !textura;

	// Entorn VGI: Activaci� del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	//	Pas de textura al shader
	if (shader_menu != CAP_SHADER) glUniform1i(glGetUniformLocation(shader_programID, "texture"), textura);

	// Entorn VGI: Desactivaci� del contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateIluminacioTextures(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (textura) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// IL.LUMINACI�->TEXTURA TEXTURES PREDEFINIDES
// TEXTURA Fusta
// TEXTURA Marbre
// TEXTURA Metall


// ILUMINACI� --> TEXTURA: C�rrega fitxer textura per llibreria SOIL
void CEntornVGIView::OnIluminacioTexturaFitxerimatge()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	CString nomf;
	t_textura = FITXERIMA;		tFlag_invert_Y = true;
	textura = true;

	// Obrir di�leg de lectura de fitxer
	CFileDialog openTextur(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("JPG Files(*.jpg)|*.jpg|Error Files (*.err)|*err|All Files (*.*)|*.*||"));;

	if (openTextur.DoModal() != IDOK)	return;                 // stay with old data file
	else nomf = openTextur.GetPathName();

	// Conversi� de la variable CString nom a la variable char *nomfitx, 
	//		compatible amb  les funcions de c�rrega de fitxers textura
	char* nomfitx = CString2Char(nomf);

	// Entorn VGI: Activaci� el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	texturesID[0] = loadIMA_SOIL(nomfitx);

	//	Pas de textura al shader
	if (shader_menu != CAP_SHADER) glUniform1i(glGetUniformLocation(shader_programID, "texture0"), GLint(0));

	// Desactivaci� contexte OpenGL: Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateIluminacioTexturaFitxerimatge(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (t_textura == FITXERIMA) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACI� --> TEXTURA: FLAG_INVERT_Y Inversi� coordenada t de textura (1-cty) per a textures SOIL (TRUE) 
//			o no (FALSE) per a objectes 3DS i OBJ.
void CEntornVGIView::OnIluminacioTexturaFlagInvertY()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	if (textura) tFlag_invert_Y = !tFlag_invert_Y;

	// Entorn VGI: Activaci� el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	//	Pas de par�metres textura al shader
	if (shader_menu != CAP_SHADER) glUniform1i(glGetUniformLocation(shader_programID, "flag_invert_y"), tFlag_invert_Y);

	// Desactivaci� contexte OpenGL: Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateIluminacioTexturaFlagInvertY(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (tFlag_invert_Y) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


/* ------------------------------------------------------------------------- */
/*					11. LLUMS												 */
/* ------------------------------------------------------------------------- */

// LLUMS: Activaci� / Desactivaci� llum ambient 
void CEntornVGIView::OnLlumsLlumambient()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	llum_ambient = !llum_ambient;

	if (shader_menu != CAP_SHADER) {
		// Entorn VGI: Activaci� del contexte OpenGL
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		// Pas m�scara llums
		glUniform1i(glGetUniformLocation(shader_programID, "sw_intensity[1]"), (llum_ambient && sw_material[1]));
		// Entorn VGI: Desactivaci� del contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateLlumsLlumambient(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (llum_ambient) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// LLUMS: Activaci� /Desactivaci� llum 0 (GL_LIGHT0)
void CEntornVGIView::OnLlumsLlum0()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	llumGL[0].encesa = !llumGL[0].encesa;

	if (shader_menu != CAP_SHADER) {
		// Entorn VGI: Activaci� del contexte OpenGL
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		// Pas m�scara llums
		glUniform1i(glGetUniformLocation(shader_programID, "sw_lights[0]"), llumGL[0].encesa);
		// Entorn VGI: Desactivaci� del contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateLlumsLlum0(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (llumGL[0].encesa) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// LLUMS-->ON/OFF: Activaci� /Desactivaci� llum 1 (GL_LIGHT1)
void CEntornVGIView::OnLlumsLlum1()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	llumGL[1].encesa = !llumGL[1].encesa;

	if (shader_menu != CAP_SHADER) {
		// Entorn VGI: Activaci� del contexte OpenGL
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		// Pas m�scara llums
		glUniform1i(glGetUniformLocation(shader_programID, "sw_lights[1]"), llumGL[1].encesa);
		// Entorn VGI: Desactivaci� del contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateLlumsLlum1(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (llumGL[1].encesa) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// LLUMS-->ON/OFF: Activaci� /Desactivaci� llum 2 (GL_LIGHT2)
void CEntornVGIView::OnLlumsLlum2()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	llumGL[2].encesa = !llumGL[2].encesa;

	if (shader_menu != CAP_SHADER) {
		// Entorn VGI: Activaci� del contexte OpenGL
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		// Pas m�scara llums
		glUniform1i(glGetUniformLocation(shader_programID, "sw_lights[2]"), llumGL[2].encesa);
		// Entorn VGI: Desactivaci� del contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateLlumsLlum2(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (llumGL[2].encesa) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// LLUMS-->ON/OFF: Activaci� /Desactivaci� llum 3 (GL_LIGHT3)
void CEntornVGIView::OnLlumsLlum3()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	llumGL[3].encesa = !llumGL[3].encesa;
	sw_il = true;

	if (shader_menu != CAP_SHADER) {
		// Entorn VGI: Activaci� del contexte OpenGL
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		// Pas m�scara llums
		glUniform1i(glGetUniformLocation(shader_programID, "sw_lights[3]"), llumGL[3].encesa);
		// Entorn VGI: Desactivaci� del contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateLlumsLlum3(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (llumGL[3].encesa) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// LLUMS-->ON/OFF: Activaci� /Desactivaci� llum 4 (GL_LIGHT4)
void CEntornVGIView::OnLlumsLlum4()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	llumGL[4].encesa = !llumGL[4].encesa;
	sw_il = true;

	if (shader_menu != CAP_SHADER) {
		// Entorn VGI: Activaci� del contexte OpenGL
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		// Pas m�scara llums
		glUniform1i(glGetUniformLocation(shader_programID, "sw_lights[4]"), llumGL[4].encesa);
		// Entorn VGI: Desactivaci� del contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateLlumsLlum4(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (llumGL[4].encesa) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


/* ------------------------------------------------------------------------- */
/*					12. SHADERS												 */
/* ------------------------------------------------------------------------- */

// SHADERS: Desactivaci� de tots els shaders
void CEntornVGIView::OnShadersSense()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	shader_menu = CAP_SHADER;
	releaseAllShaders();

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateShadersSense(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (shader_menu == CAP) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// SHADERS: Iluminaci� Gouraud amb Shaders
void CEntornVGIView::OnShadersGouraud()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	bool apagat = false;
	if (sw_shader) releaseAllShaders();
	else sw_shader = true;
	shader_menu = GOURAUD_SHADER;

	// Entorn VGI: Activaci� del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	shader_programID = shaderLighting.loadFileShaders("./shaders/gouraud_shdrML.VERT", "./shaders/gouraud_shdrML.FRAG");

	// Entorn VGI: Desactivaci� del contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateShadersGouraud(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (shader_menu == GOURAUD_SHADER) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// SHADERS: Iluminaci� Phong amb Shaders
void CEntornVGIView::OnShadersPhong()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	bool apagat = false;
	if (sw_shader) releaseAllShaders();		// Eliminar Programs previs
	else sw_shader = true;
	shader_menu = PHONG_SHADER;

	// Entorn VGI: Activaci� del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	// Carregar shaders phong_shdrML.vert i phong_shdrML.frag en Program shader_program
	shader_programID = shaderLighting.loadFileShaders("./shaders/phong_shdrML.VERT", "./shaders/phong_shdrML.FRAG");

	// Entorn VGI: Desactivaci� del contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateShadersPhong(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (shader_menu == PHONG_SHADER) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// SHADERS: C�rrega Fitxers Shader
void CEntornVGIView::OnShaderLoadFiles()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	CString nomVert, nomFrag;

	if (sw_shader) releaseAllShaders();
	else sw_shader = true;
	shader_menu = FILE_SHADER;

	// C�rrega fitxer VERT
	// Entorn VGI: Obrir di�leg de lectura de fitxer (fitxers (*.VERT)
	CFileDialog openVert(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("VERT Files(*.vert)|*.vert|All Files (*.*)|*.*||"));;

	if (openVert.DoModal() != IDOK)
		return;                 // stay with old data file
	else nomVert = openVert.GetPathName();
	// Entorn VGI: Variable de tipus CString 'nomVert' cont� el nom del fitxer seleccionat

	// Entorn VGI: Conversi� de la variable CString nom a la variable char *nomfitx, 
	//		compatible amb  les funcions de c�rrega de fitxers fractals
	//	char *nomfitxV = CString2Char(nomVert);

	// Conversi� CString nomFrag a--> std::string nomFragS
	std::string nomVertS = CString2String(nomVert);

	// C�rrega fitxer FRAG
	// Entorn VGI: Obrir di�leg de lectura de fitxer (fitxers (*.VERT)
	CFileDialog openFrag(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("FRAG Files(*.frag)|*.frag|All Files (*.*)|*.*||"));;

	if (openFrag.DoModal() != IDOK)
		return;                 // stay with old data file
	else nomFrag = openFrag.GetPathName();
	// Entorn VGI: Variable de tipus CString 'nomFrag' cont� el nom del fitxer seleccionat

// Entorn VGI: Conversi� de la variable CString nom a la variable char *nomfitx, 
//		compatible amb  les funcions de c�rrega de fitxers fractals
//	char *nomfitxF = CString2Char(nomFrag);

	// Conversi� CString nomFrag a--> std::string nomFragS
	std::string nomFragS = CString2String(nomFrag);

	// Entorn VGI: Activaci� del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	shader_programID = shaderLighting.loadFileShadersTest(nomVertS, nomFragS);

	// Entorn VGI: Desactivaci� del contexte OpenGL. Permet la coexistencia d'altres contextes de generaci�
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);

}


void CEntornVGIView::OnUpdateShaderLoadFiles(CCmdUI* pCmdUI)
{
	// TODO: Agregue aqu� su c�digo de controlador de IU para actualizaci�n de comandos
	if (shader_menu == FILE_SHADER) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}



/* ------------------------------------------------------------------------- */
/*   RECURSOS DE BOTONS (ToolBar) DE L'APLICACIO                             */
/* ------------------------------------------------------------------------- */
// Incloure funcions de tractament de botons


// ---------------- Entorn VGI: Funcions locals a EntornVGIView.cpp

// Log2: C�lcul del log base 2 de num
int CEntornVGIView::Log2(int num)
{
	int tlog;

	if (num >= 8192) tlog = 13;
	else if (num >= 4096) tlog = 12;
	else if (num >= 2048) tlog = 11;
	else if (num >= 1024) tlog = 10;
	else if (num >= 512) tlog = 9;
	else if (num >= 256) tlog = 8;
	else if (num >= 128) tlog = 7;
	else if (num >= 64) tlog = 6;
	else if (num >= 32) tlog = 5;
	else if (num >= 16) tlog = 4;
	else if (num >= 8) tlog = 3;
	else if (num >= 4) tlog = 2;
	else if (num >= 2) tlog = 1;
	else tlog = 0;

	return tlog;
}


// CString2char: Funci� de conversi� de variable CString a char * per a noms de fitxers 
char* CEntornVGIView::CString2Char(CString entrada)
{
	//char * par_sortida=" ";
	// Variable de tipus CString 'entrada' cont� un string tipus CString
	//-------------------------------------------------------------------------------------
	// Conversi� de la variable CString entrada a la variable char *sortida, 
	//	compatible amb la funci� Carregar3DS, de c�rrega de fitxers 3DS
	//	char * nomfitx = (char *)(LPCTSTR)nom;

	// Conversi� variable w_char --> char *
	//	char *nomf="  ";
	//	wcstombs(nomf,strFileName.GetBuffer(3),90);
	//	char *nomf = reinterpret_cast<char *> (nom.GetBuffer(3));

	size_t origsize = wcslen(entrada.GetBuffer(3)) + 1;
	size_t convertedChars = 0;

	// Use a multibyte string to append the type of string
	// to the new string before displaying the result.
	char strConcat[] = " (char *)";
	size_t strConcatsize = (strlen(strConcat) + 1) * 2;

	// Allocate two bytes in the multibyte output string for every wide
	// character in the input string (including a wide character
	// null). Because a multibyte character can be one or two bytes,
	// you should allot two bytes for each character. Having extra
	// space for the new string is not an error, but having
	// insufficient space is a potential security problem.
	const size_t newsize = origsize * 2;
	// The new string will contain a converted copy of the original
	// string plus the type of string appended to it.
	//	char *nomfitx = new char[newsize + strConcatsize];
	char* par_sortida = new char[newsize + strConcatsize];

	// Put a copy of the converted string into nstring
	wcstombs_s(&convertedChars, par_sortida, newsize, entrada.GetBuffer(3), _TRUNCATE);
	// append the type of string to the new string.
	//----------------------------------------------------------------------------------

	// Variable de tipus char *nomfitx cont� el nom del fitxer seleccionat
	return par_sortida;
}


// Refl_MaterialOff: Desactivar Reflexi� de Material
void CEntornVGIView::Refl_MaterialOff()
{
	sw_material_old[0] = sw_material[0];	sw_material[0] = false;
	sw_material_old[1] = sw_material[1];	sw_material[1] = false;
	sw_material_old[2] = sw_material[2];	sw_material[2] = false;
	sw_material_old[3] = sw_material[3];	sw_material[3] = false;
}

// Refl_MaterialOn: Activar Reflexi� de Material
void CEntornVGIView::Refl_MaterialOn()
{
	sw_material[0] = sw_material_old[0];
	sw_material[1] = sw_material_old[1];
	sw_material[2] = sw_material_old[2];
	sw_material[3] = sw_material_old[3];
}


// -------------------- FUNCIONS CORBES SPLINE i BEZIER

// llegir_ptsC: Llegir punts de control de corba (spline o Bezier) d'un fitxer .crv. 
//				Retorna el nombre de punts llegits en el fitxer.
//int llegir_pts(CString nomf)
int CEntornVGIView::llegir_ptsC(char* nomf)
{
	int i, j;
	FILE* fd;

	// Inicialitzar vector punts de control de la corba spline
	for (i = 0; i < MAX_PATCH_CORBA; i = i++)
	{
		PC_t[i].x = 0.0;
		PC_t[i].y = 0.0;
		PC_t[i].z = 0.0;
	}

	//	ifstream f("altinicials.dat",ios::in);
	//    f>>i; f>>j;
	if ((fd = fopen(nomf, "rt")) == NULL)
	{
		LPCWSTR texte1 = reinterpret_cast<LPCWSTR> ("ERROR:");
		LPCWSTR texte2 = reinterpret_cast<LPCWSTR> ("File .crv was not opened");
		//MessageBox(NULL, texte1, texte2, MB_OK);
		MessageBox(texte1, texte2, MB_OK);
		//AfxMessageBox("file was not opened");
		return false;
	}
	fscanf(fd, "%d \n", &i);
	if (i == 0) return false;
	else {
		for (j = 0; j < i; j = j++)
		{	//fscanf(fd, "%f", &corbaSpline[j].x);
			//fscanf(fd, "%f", &corbaSpline[j].y);
			//fscanf(fd, "%f \n", &corbaSpline[j].z);
			fscanf(fd, "%lf %lf %lf \n", &PC_t[j].x, &PC_t[j].y, &PC_t[j].z);

		}
	}
	fclose(fd);

	return i;
}

// Conversi� CString --> std::string
std::string CEntornVGIView::CString2String(const CString& cString)
{
	std::string strStd;

	for (int i = 0; i < cString.GetLength(); ++i)
	{
		if (cString[i] <= 0x7f)
			strStd.append(1, static_cast<char>(cString[i]));
		else
			strStd.append(1, '?');
	}

	return strStd;
}



void CEntornVGIView::OnObjecteTetris()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	objecte = OBJOBJ;	textura = false;		tFlag_invert_Y = true; tetris = true;

	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);


	CString nom[15] = {
		CString(_T("..\\..\\objects\\fig1_color.obj")),
		CString(_T("..\\..\\objects\\fig2_color.obj")),
		CString(_T("..\\..\\objects\\fig3_color.obj")),
		CString(_T("..\\..\\objects\\fig4_color.obj")),
		CString(_T("..\\..\\objects\\fig5_color.obj")),
		CString(_T("..\\..\\objects\\fig1_cub_lightblue.obj")),
		CString(_T("..\\..\\objects\\fig2_cub_yellow.obj")),
		CString(_T("..\\..\\objects\\fig3_cub_red.obj")),
		CString(_T("..\\..\\objects\\fig4_cub_purple.obj")),
		CString(_T("..\\..\\objects\\fig5_cub_green.obj")),
		CString(_T("..\\..\\objects\\fig_tauler.obj"))
	};
	for (int i = 0; i < 11; i++) {

		char* nomfitx = CString2Char(nom[i]);
		if (ObOBJ == NULL) ObOBJ = new COBJModel;
		if (idVao[i] != 0) deleteVAOList(I + i);
		idVao[i] = ObOBJ->LoadModel(nomfitx, I + i);
		if (i < 5) {
			Piece piece(I + i);
			piece.posIni();
			pieces[i] = piece;
		}
		if ((i >= 5) && (i != 10))
		{
			Block block(-1, -1, -1);
			block.setIdVao(I + i);
			vaoBlocks[i - 5] = block;
		}
		if (i == 10)
		{
			m_board.setIdVao(I + i);
		}
	}
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);	// Desactivem contexte OpenGL

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);

}
void CEntornVGIView::mainTetris() {
	OnObjecteTetris();
	tetris = true;
	srand(time(nullptr));

	int tamanyPe = sizeof(pieces) / sizeof(pieces[0]);
	pieces[numPiece].posIni();
	if (tetris) {
		SetTimer(WM_TIMER, 1000, NULL);
	}
}