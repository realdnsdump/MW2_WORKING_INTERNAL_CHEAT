#include <cstdint>
#include "test.h"
#include "imgui/Kiero/kiero.h"
#include "imgui_draw.h"
#include "game.h"
#include<winnt.h>
#include"memory.h"
#include "esp.h"
#include "aimbot.h"

#include "globals.h"
#include "settings.h"
#include "style.h"

typedef long(__fastcall* Present)(IDXGISwapChain*, UINT, UINT);
static Present ori_present = NULL;
void WndProc_hk();

typedef LRESULT(CALLBACK* tWndProc)(HWND hWnd, UINT Msg, WPARAM wp, LPARAM lp);
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace d3d12test
{
	ID3D12CommandQueue* d3d12CommandQueue2 = nullptr;
	ID3D12CommandQueue* d3d12CommandQueueoriginal = nullptr;
}
namespace d3d12
{

	IDXGISwapChain3* pSwapChain;
	ID3D12Device* pDevice;
	ID3D12CommandQueue* pCommandQueue;
	ID3D12Fence* pFence;
	ID3D12DescriptorHeap* d3d12DescriptorHeapBackBuffers = nullptr;
	ID3D12DescriptorHeap* d3d12DescriptorHeapImGuiRender = nullptr;
	ID3D12DescriptorHeap* pSrvDescHeap = nullptr;;
	ID3D12DescriptorHeap* pRtvDescHeap = nullptr;;
	ID3D12GraphicsCommandList* pCommandList;

	FrameContext* FrameContextArray;
	ID3D12Resource** pID3D12ResourceArray;
	D3D12_CPU_DESCRIPTOR_HANDLE* RenderTargetDescriptorArray;

	HANDLE hSwapChainWaitableObject;
	HANDLE hFenceEvent;

	UINT NUM_FRAMES_IN_FLIGHT;
	UINT NUM_BACK_BUFFERS;

	UINT   frame_index = 0;
	UINT64 fenceLastSignaledValue = 0;
}

void unhookPresent();
void hookpresent();

namespace imgui
{
	bool is_ready;
	bool is_need_reset_imgui;

	bool IsReady()
	{
		return is_ready;
	}

	void reset_imgui_request()
	{
		is_need_reset_imgui = true;
	}

	__forceinline bool get_is_need_reset_imgui()
	{
		return is_need_reset_imgui;
	}

	void init_d3d12(IDXGISwapChain3* pSwapChain, ID3D12CommandQueue* pCommandQueue)
	{

		d3d12::pSwapChain = pSwapChain;
		d3d12::pCommandQueue = pCommandQueue;

		if (!SUCCEEDED(d3d12::pSwapChain->GetDevice(__uuidof(ID3D12Device), (void**)&d3d12::pDevice)))
			Exit();

		{
			DXGI_SWAP_CHAIN_DESC1 desc;

			if (!SUCCEEDED(d3d12::pSwapChain->GetDesc1(&desc)))
				Exit();

			d3d12::NUM_BACK_BUFFERS = desc.BufferCount;
			d3d12::NUM_FRAMES_IN_FLIGHT = desc.BufferCount;
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.NumDescriptors = d3d12::NUM_BACK_BUFFERS;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask = 1;

			if (!SUCCEEDED(d3d12::pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&d3d12::pRtvDescHeap))))
				Exit();
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.NumDescriptors = 1;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			desc.NodeMask = 0;

			if (!SUCCEEDED(d3d12::pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&d3d12::pSrvDescHeap))))
				Exit();
		}

		if (!SUCCEEDED(d3d12::pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3d12::pFence))))
			Exit();

		d3d12::FrameContextArray = new FrameContext[d3d12::NUM_FRAMES_IN_FLIGHT];
		d3d12::pID3D12ResourceArray = new ID3D12Resource * [d3d12::NUM_BACK_BUFFERS];
		d3d12::RenderTargetDescriptorArray = new D3D12_CPU_DESCRIPTOR_HANDLE[d3d12::NUM_BACK_BUFFERS];

		for (UINT i = 0; i < d3d12::NUM_FRAMES_IN_FLIGHT; ++i)
		{
			if (!SUCCEEDED(d3d12::pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&d3d12::FrameContextArray[i].CommandAllocator))))
				Exit();
		}

		SIZE_T nDescriptorSize = d3d12::pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d12::pRtvDescHeap->GetCPUDescriptorHandleForHeapStart();

		for (UINT i = 0; i < d3d12::NUM_BACK_BUFFERS; ++i)
		{
			d3d12::RenderTargetDescriptorArray[i] = rtvHandle;
			rtvHandle.ptr += nDescriptorSize;
		}


		if (!SUCCEEDED(d3d12::pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, d3d12::FrameContextArray[0].CommandAllocator, NULL, IID_PPV_ARGS(&d3d12::pCommandList))) ||
			!SUCCEEDED(d3d12::pCommandList->Close()))
		{
			Exit();
		}


		d3d12::hSwapChainWaitableObject = d3d12::pSwapChain->GetFrameLatencyWaitableObject();

		d3d12::hFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		if (d3d12::hFenceEvent == NULL)
			Exit();


		ID3D12Resource* pBackBuffer;
		for (UINT i = 0; i < d3d12::NUM_BACK_BUFFERS; ++i)
		{
			if (!SUCCEEDED(d3d12::pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer))))
				Exit();

			d3d12::pDevice->CreateRenderTargetView(pBackBuffer, NULL, d3d12::RenderTargetDescriptorArray[i]);
			d3d12::pID3D12ResourceArray[i] = pBackBuffer;
		}
	}

	void _clear()
	{
		d3d12::pSwapChain = nullptr;
		d3d12::pDevice = nullptr;
		d3d12::pCommandQueue = nullptr;

		if (d3d12::pFence)
		{
			d3d12::pFence->Release();
			d3d12::pFence = nullptr;
		}

		if (d3d12::pSrvDescHeap)
		{
			d3d12::pSrvDescHeap->Release();
			d3d12::pSrvDescHeap = nullptr;
		}

		if (d3d12::pRtvDescHeap)
		{
			d3d12::pRtvDescHeap->Release();
			d3d12::pRtvDescHeap = nullptr;
		}

		if (d3d12::pCommandList)
		{
			d3d12::pCommandList->Release();
			d3d12::pCommandList = nullptr;
		}

		if (d3d12::FrameContextArray)
		{
			for (UINT i = 0; i < d3d12::NUM_FRAMES_IN_FLIGHT; ++i)
			{
				if (d3d12::FrameContextArray[i].CommandAllocator)
				{
					d3d12::FrameContextArray[i].CommandAllocator->Release();
					d3d12::FrameContextArray[i].CommandAllocator = nullptr;
				}
			}

			delete[] d3d12::FrameContextArray;
			d3d12::FrameContextArray = NULL;
		}

		if (d3d12::pID3D12ResourceArray)
		{
			for (UINT i = 0; i < d3d12::NUM_BACK_BUFFERS; ++i)
			{
				if (d3d12::pID3D12ResourceArray[i])
				{
					d3d12::pID3D12ResourceArray[i]->Release();
					d3d12::pID3D12ResourceArray[i] = nullptr;
				}
			}

			delete[] d3d12::pID3D12ResourceArray;
			d3d12::pID3D12ResourceArray = NULL;
		}

		if (d3d12::RenderTargetDescriptorArray)
		{
			delete[] d3d12::RenderTargetDescriptorArray;
			d3d12::RenderTargetDescriptorArray = NULL;
		}


		if (d3d12::hSwapChainWaitableObject)
		{
			d3d12::hSwapChainWaitableObject = nullptr;
		}

		if (d3d12::hFenceEvent)
		{
			CloseHandle(d3d12::hFenceEvent);
			d3d12::hFenceEvent = nullptr;
		}


		d3d12::NUM_FRAMES_IN_FLIGHT = 0;
		d3d12::NUM_BACK_BUFFERS = 0;


		d3d12::frame_index = 0;
	}

	void clear()
	{
		if (d3d12::FrameContextArray)
		{
			FrameContext* frameCtxt = &d3d12::FrameContextArray[d3d12::frame_index % d3d12::NUM_FRAMES_IN_FLIGHT];

			UINT64 fenceValue = frameCtxt->FenceValue;

			if (fenceValue == 0)
				return; // No fence was signaled

			frameCtxt->FenceValue = 0;

			bool bNotWait = d3d12::pFence->GetCompletedValue() >= fenceValue;

			if (!bNotWait)
			{
				d3d12::pFence->SetEventOnCompletion(fenceValue, d3d12::hFenceEvent);

				WaitForSingleObject(d3d12::hFenceEvent, INFINITE);
			}

			_clear();
		}
	}

	FrameContext* WaitForNextFrameResources()
	{
		UINT nextFrameIndex = d3d12::frame_index + 1;
		d3d12::frame_index = nextFrameIndex;

		HANDLE waitableObjects[] = { d3d12::hSwapChainWaitableObject, NULL };
		constexpr DWORD numWaitableObjects = 1;

		FrameContext* frameCtxt = &d3d12::FrameContextArray[nextFrameIndex % d3d12::NUM_FRAMES_IN_FLIGHT];

		WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

		return frameCtxt;
	}

	void reinit(IDXGISwapChain3* pSwapChain, ID3D12CommandQueue* pCommandQueue)
	{
		init_d3d12(pSwapChain, pCommandQueue);
		ImGui_ImplDX12_CreateDeviceObjects();
	}

	ImFont* begin_imgui(IDXGISwapChain3* pSwapChain, ID3D12CommandQueue* pCommandQueue, type::tImguiStyle SetStyleFunction)
	{
		static ImFont* s_main_font;

		if (is_ready && get_is_need_reset_imgui())
		{
			reinit(pSwapChain, pCommandQueue);
			is_need_reset_imgui = false;
		}

		if (is_ready)
			return s_main_font;

		init_d3d12(pSwapChain, pCommandQueue);

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		if (SetStyleFunction == nullptr)
			ImGui::StyleColorsDark();
		else
			SetStyleFunction();

		ImGui_ImplWin32_Init(g_data::hWind);
		ImGui_ImplDX12_Init(
			d3d12::pDevice,
			d3d12::NUM_FRAMES_IN_FLIGHT,
			DXGI_FORMAT_R8G8B8A8_UNORM, d3d12::pSrvDescHeap,
			d3d12::pSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
			d3d12::pSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

		ImFont* main_font = io.Fonts->AddFontFromMemoryTTF(tahoma_ttf, sizeof(tahoma_ttf), 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());

		if (main_font == nullptr)
			Exit();

		s_main_font = main_font;

		WndProc_hk();

		is_ready = true;

		return s_main_font;
	}
}

void(*oExecuteCommandListsD3D12)(ID3D12CommandQueue*, UINT, ID3D12CommandList*);

int Tabs = 0;
bool choice = false;
bool endKeyPreviouslyPressed = false;
auto vec_files = Settings::GetList();

void render_SliderFloat(const char* id, float& value, float lineLength, float minValue, float maxValue, ImU32 leftColor, const char* format)
{

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImVec2 linePos = window->DC.CursorPos;
	ImVec2 lineSize(lineLength, 4.0f);

	float circleRadius = 8.0f;
	float lineStartX = linePos.x;
	float lineEndX = linePos.x + lineSize.x;
	float clampedValue = std::clamp(value, minValue, maxValue);
	float circlePosX = lineStartX + (lineEndX - lineStartX) * ((clampedValue - minValue) / (maxValue - minValue));
	ImVec2 circlePos(circlePosX, linePos.y + lineSize.y / 2.0f);

	ImVec2 lineStartPos = linePos;
	ImVec2 lineEndPos = ImVec2(linePos.x + lineSize.x, linePos.y + lineSize.y);

	ImVec2 filledStartPos = lineStartPos;
	ImVec2 filledEndPos = ImVec2(circlePos.x, lineEndPos.y);
	window->DrawList->AddRectFilled(filledStartPos, filledEndPos, leftColor);

	ImVec2 lineStartPosRight = ImVec2(circlePos.x, lineStartPos.y);
	ImVec2 lineEndPosRight = lineEndPos;
	window->DrawList->AddRectFilled(lineStartPosRight, lineEndPosRight, IM_COL32(128, 128, 128, 255));

	ImGui::SetCursorScreenPos(ImVec2(circlePos.x - circleRadius, circlePos.y - circleRadius));
	ImGui::PushID(id);
	ImGui::InvisibleButton("##circle", ImVec2(circleRadius * 2.0f, circleRadius * 2.0f));
	ImGui::PopID();

	static bool activeSlider = false;

	if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
	{
		if (!activeSlider)
		{
			activeSlider = true;
			value = clampedValue;
		}

		ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;
		float deltaValue = (mouseDelta.x / (lineEndX - lineStartX)) * (maxValue - minValue);
		value += deltaValue;
		value = std::clamp(value, minValue, maxValue);
	}
	else if (!ImGui::IsMouseDown(0))
	{
		activeSlider = false;
	}

	window->DrawList->AddCircleFilled(circlePos, circleRadius, IM_COL32(255, 255, 255, 255));

	ImGui::SetCursorScreenPos(ImVec2(linePos.x + lineSize.x / 2.0f, linePos.y - ImGui::GetTextLineHeight() / 2.0f));
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), format, value);
	ImGui::PopFont();
}

void menu()
{
	if (GetAsyncKeyState(VK_INSERT) & 0x1)
	{
		globals::b_menu_open = !globals::b_menu_open;
	}
	if (GetAsyncKeyState(VK_END))
	{
		if (!endKeyPreviouslyPressed)
		{
			globals::menu_available = !globals::menu_available;  // Toggle the value
		}
		endKeyPreviouslyPressed = true;
	}
	else
	{
		endKeyPreviouslyPressed = false;
	}
	if (GetAsyncKeyState(VK_PRIOR))
	{
		kiero::unbind(140);
		kiero::shutdown();
	}

	if (GetAsyncKeyState(VK_F1))
	{
		std::string buf = XOR("Profile_1");

		if (!vec_files.empty())
			Settings::Load_Settings(buf);
	}

	if (GetAsyncKeyState(VK_F2))
	{
		std::string profile2 = XOR("Profile_2");

		if (!vec_files.empty())
			Settings::Load_Settings(profile2);
	}

	if (GetAsyncKeyState(VK_F3))
	{
		std::string buf2 = XOR("Profile_3");

		if (!vec_files.empty())
			Settings::Load_Settings(buf2);
	}

	EditorColorScheme::ApplyTheme2();

	if (choice == false && screenshot::visuals)
	{
		ImGui::SetNextWindowSize(ImVec2(180, 100), ImGuiCond_Always);
		ImGui::Begin(XOR("PlatForm Selection"), &globals::b_menu_open, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar);

		static int battlenet = 0;
		static int steam = 0;
		static bool animate = true;

		ImGui::Text("Select Platform Please...");
		ImGui::Spacing();
		if (ImGui::Button(XOR("Battle.net"), ImVec2(75, 25)))
			battlenet++;
		ImGui::SameLine();
		if (ImGui::Button(XOR("steam"), ImVec2(75, 25)))
			steam++;

		if (battlenet & 1)
		{
			static float progress = 0.0f, progress_dir = 1.0f;
			if (animate)
			{
				progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
				if (progress >= +1.1f) { progress = +1.1; }
				if (progress <= -0.1f) { progress = -0.1f; }
			}
			ImGui::Spacing();
			ImGui::ProgressBar(progress, ImVec2(125, 22.5f));

			float progress_saturated = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;
			char buf[32];
			sprintf_s(buf, XOR("%d/%d"), (int)(progress_saturated * 1753), 1753);

			if (progress >= 1.1f)
			{
				globals::platform = 0;
				choice = true;
			}
		}
		else if (steam & 1)
		{
			static float progress = 0.0f, progress_dir = 1.0f;
			if (animate)
			{
				progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
				if (progress >= +1.1f) { progress = +1.1; }
				if (progress <= -0.1f) { progress = -0.1f; }
			}
			ImGui::Spacing();
			ImGui::ProgressBar(progress, ImVec2(125, 22.5f));

			float progress_saturated = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;
			char buf[32];
			sprintf_s(buf, XOR("%d/%d"), (int)(progress_saturated * 1753), 1753);

			if (progress >= 1.1f)
			{
				globals::platform = 1;
				choice = true;
			}
		}
	}

	if (choice == true && globals::b_menu_open && screenshot::visuals)
	{
		static int activeTab = 0;
		static int previousTab = -1;
		static int previoussubTab = -1;
		static int visualsubTab = 0;

		ImGui::SetNextWindowSize(ImVec2(615, 465));
		ImGui::Begin(XOR(""), &globals::b_menu_open, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize);

		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = ImGui::GetStyle().Colors;
		auto previousWP = style->WindowPadding;
		auto previousFBS = style->FrameBorderSize;
		auto previousFR = style->FrameRounding;
		auto previousCBG = colors[ImGuiCol_ChildBg];
		style->WindowPadding = ImVec2(0.f, 0.f);
		style->FrameBorderSize = 0.0f;
		style->FrameRounding = 0.0f;
		colors[ImGuiCol_ChildBg] = ImVec4(0.22f, 0.22f, 0.22f, 0.00f);

		ImGui::SetCursorPos(ImVec2(-1, 0));
		if (ImGui::BeginChild("##TABS", ImVec2(75, 430)), ImGuiWindowFlags_NoBackground)
		{
			ImGui::SetCursorPosY(55.0f);
			if (ImGui::Button("Aimbot", ImVec2(65, 40)))
			{
				previousTab = activeTab;
				activeTab = 0;
			}

			ImGui::Spacing();

			if (ImGui::Button("Visuals", ImVec2(65, 40)))
			{
				previousTab = activeTab;
				activeTab = 1;
				visualsubTab = 0;
			}

			ImGui::Spacing();

			if (ImGui::Button("Misc", ImVec2(65, 40)))
			{
				previousTab = activeTab;
				activeTab = 2;
			}

			if (ImGui::Button("Configs", ImVec2(65, 40)))
			{
				previousTab = activeTab;
				activeTab = 3;
			}

			if (ImGui::Button("Menu S", ImVec2(65, 40)))
			{
				previousTab = activeTab;
				activeTab = 4;
			}
		}
		style->WindowPadding = previousWP;
		style->FrameBorderSize = previousFBS;
		style->FrameRounding = previousFR;
		colors[ImGuiCol_ChildBg] = previousCBG;
		ImGui::EndChild();


		style->WindowPadding = ImVec2(3.f, 3.f);


		switch (activeTab)
		{
		case 0:
			colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.12f, 0.12f, 0.67f);
			style->ChildBorderSize = 0.f;
			ImGui::SetCursorPos(ImVec2(70, 40));
			if (ImGui::BeginChild("##aimbotchildmain", ImVec2(530, 410), true))
			{
				ImGui::SetCursorPos(ImVec2(10, 60));
				ImGui::BeginChild(XOR("##aimbotchild0"), ImVec2(247, 340), true);
				ImGui::Checkbox(XOR("Enable Aimbot"), &globals::b_lock);
				ImGui::Checkbox(XOR("Disable Visible Check"), &globals::b_visible);
				ImGui::Checkbox(XOR("Prediction"), &globals::prediction);
				ImGui::Checkbox(XOR("Disable Before Down"), &globals::disable_bd);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(XOR("Will Disable aimbot after player loses half of health"));
				ImGui::Checkbox(XOR("Aim at Downed"), &globals::b_skip_knocked);
				ImGui::Checkbox(XOR("Aim at Self Revive"), &globals::self_revive);
				ImGui::Checkbox(XOR("Aim at Being Revived"), &globals::b_revive);
				ImGui::Checkbox(XOR("Show FOV"), &globals::b_fov);
				ImGui::SameLine(215);
				ImGui::ColorEdit4(XOR("##fov Color4"), (float*)&color::showfov, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(XOR("FOV Color"));
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::SetCursorPosX(25);
				render_SliderFloat("##FOV_SIZE", globals::f_fov_size, 200, 10.0f, 1200.0f, customization::slider, "FOV Size: %0.0f");
				ImGui::EndChild();

				ImGui::SetCursorPos(ImVec2(275, 60));
				ImGui::BeginChild(XOR("##aimbotchild1"), ImVec2(247, 340), true);
				ImGui::TextDisabled(XOR("Bone Selection"));
				ImGui::Checkbox(XOR("Bone Scan"), &globals::bone_scan);
				ImGui::Checkbox(XOR("Smart Bone Selection"), &globals::smart_bone);
				ImGui::PushItemWidth(200);
				ImGui::Combo(XOR("##MAINBONE"), &globals::bone_index, globals::aim_lock_point, 5);
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::TextDisabled(XOR("Aimbot Method"));
				ImGui::Combo(XOR("##PRIOR"), &globals::priority, globals::i_priority, 2);
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::TextDisabled("Lock Method");
				ImGui::Combo("##LOCKONMETHOD", &globals::lockon, globals::lockonmethodss, 2);
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::SetCursorPosX(25);
				render_SliderFloat("##AIM_SMOOTH", globals::aim_smooth, 200, 1.0f, 5.0f, customization::slider, "Smoothness: %0.1f");
				ImGui::Spacing();
				ImGui::Text("");
				ImGui::SetCursorPosX(25);
				render_SliderFloat("##AIM_DIST", globals::f_min_closest, 200, 1.0f, 1000.0f, customization::slider, "Aim Distance: %0.1fm");

				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(XOR("Aim At Enemy Distance"));
				ImGui::EndChild();
			}

			colors[ImGuiCol_ChildBg] = previousCBG;
			ImGui::EndChild();
			break;
		case 1:
			colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.12f, 0.12f, 0.67f);
			style->ChildBorderSize = 0.f;
			ImGui::SetCursorPos(ImVec2(70, 40));
			if (ImGui::BeginChild("##visualchildmain", ImVec2(530, 410), true))
			{
				//nav bar pages { Main Visuals, ESP Customization , ESP Colors }
				ImGui::SetCursorPos(ImVec2(15, 10));
				if (ImGui::BeginChild("##visuals_pages", ImVec2(500, 45), true))
				{
					if (ImGui::Button("Main Visuals"))
					{
						previoussubTab = visualsubTab;
						visualsubTab = 0;
					}

					ImGui::SameLine();

					if (ImGui::Button("ESP Custimization"))
					{
						previoussubTab = visualsubTab;
						visualsubTab = 1;
					}

					ImGui::SameLine();

					if (ImGui::Button("ESP Colors"))
					{
						previoussubTab = visualsubTab;
						visualsubTab = 2;
					}
				}
				ImGui::EndChild();

				switch (visualsubTab)
				{
				case 0:
					ImGui::SetCursorPos(ImVec2(10, 60));
					if (ImGui::BeginChild("##VISUALSUBTAB1", ImVec2(247, 340), true))
					{
						ImGui::Checkbox(XOR("Show only Visible"), &globals::vis_only);
						ImGui::Checkbox(XOR("Show Team"), &globals::b_friendly);
						ImGui::Checkbox(XOR("Snap Lines"), &globals::b_line);
						ImGui::Checkbox(XOR("Skeletons"), &globals::b_skeleton);
						ImGui::Checkbox(XOR("Head Circle"), &globals::head_circle);
					}
					ImGui::EndChild();

					ImGui::SetCursorPos(ImVec2(275, 60));
					if (ImGui::BeginChild("##VISUALSUBTAB2", ImVec2(247, 340), true))
					{
						ImGui::TextDisabled("Player ESP");
						ImGui::Checkbox(XOR("Name"), &globals::b_namee);
						ImGui::Checkbox(XOR("HealthBar"), &globals::draw_health);
						ImGui::Checkbox(XOR("Distance"), &globals::distance);
						ImGui::Checkbox(XOR("Team ID"), &globals::team_id);

						ImGui::Spacing();

						ImGui::TextDisabled(XOR("Player ESP Boxes"));
						ImGui::Checkbox(XOR("2D Box"), &globals::b_box);
						ImGui::Checkbox(XOR("Diamond"), &globals::esp_box);
						ImGui::Checkbox(XOR("Box Fill"), &globals::box_bg);

					}
					ImGui::EndChild();
					break;
				case 1:
					ImGui::SetCursorPos(ImVec2(10, 60));
					if (ImGui::BeginChild("##VISUALSUBTAB3", ImVec2(247, 340), true))
					{
						ImGui::TextDisabled(XOR("Snap Line Style"));
						ImGui::PushItemWidth(200);
						ImGui::SliderInt(XOR("##LINE STYLE"), &globals::linetype, 1, 2);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("1 = Bottom Screen\n2= Center Screen"));
						ImGui::NewLine();
						ImGui::SetCursorPosX(25);
						render_SliderFloat("##linethickness", globals::lines_thickness, 200, 1.f, 5.0f, customization::slider, "Line Thickness: %.1f");

						ImGui::Spacing();
						ImGui::NewLine();

						ImGui::TextDisabled(XOR("Crosshair"));

						ImGui::Checkbox(XOR("Show crosshair"), &globals::b_crosshair);
						ImGui::SameLine(215);
						ImGui::ColorEdit4(XOR("##crosshair Color4"), (float*)&color::crosshair, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("CrossHair Color"));
						ImGui::Combo(XOR("##crosshair type"), &globals::b_crosshair_type, globals::b_crosshairswitch, 5);

						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Spacing();

						ImGui::SetCursorPosX(20);
						render_SliderFloat("##crosshairthickness", globals::b_crosshair_thickness, 200, 1.f, 5.f, customization::slider, "Thickness: %.1f");

						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Spacing();

						ImGui::SetCursorPosX(20);
						render_SliderFloat("##crosshairsize", globals::b_crosshair_size, 200, 1.f, 30.f, customization::slider, "Size: %0.1f");
					}
					ImGui::EndChild();

					ImGui::SetCursorPos(ImVec2(275, 60));
					if (ImGui::BeginChild(XOR("##VISUALSUBTAB4"), ImVec2(247, 340), true))
					{
						ImGui::SetCursorPos(ImVec2(10, 10));
						render_SliderFloat("##maxdistanceesp", globals::max_distance, 200, 1.f, 1000.f, customization::slider, "Max Distance: %.1f");

						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Spacing();

						ImGui::SetCursorPos(ImVec2(10, 40));
						render_SliderFloat("##boxthickness", globals::box_thickness, 200, 1.f, 5.f, customization::slider, "Box Thickness: %.1f");

						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Spacing();

						ImGui::SetCursorPos(ImVec2(10, 70));
						render_SliderFloat("##bonesthickness", globals::bones_thickness, 200, 1.f, 5.f, customization::slider, "Bone Thickness: %.1f");

						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Spacing();

						ImGui::SetCursorPos(ImVec2(10, 100));
						render_SliderFloat("##textespsize", globals::esp_text_size, 200, 1.f, 20, customization::slider, "ESP Text Size: %.1f");
					}
					ImGui::EndChild();
					break;
				case 2:
					ImGui::SetCursorPos(ImVec2(10, 60));
					ImGui::BeginChild(XOR("##colorchild0"), ImVec2(247, 340), true);
					ImGui::Text(XOR(""));
					ImGui::SameLine(125);
					ImGui::TextDisabled(XOR("Enemy"));
					ImGui::Separator();
					ImGui::Text(XOR("SnapLines"));
					ImGui::SameLine(125);
					ImGui::ColorEdit4(XOR("##V E lines"), (float*)&color::V_Line_Enemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Visible Enemy"));
					ImGui::SameLine();
					ImGui::ColorEdit4(XOR("##NV E lines"), (float*)&color::NV_Line_Enemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Not Visible Enemy"));
					ImGui::Text(XOR("Skeletons"));
					ImGui::SameLine(125);
					ImGui::ColorEdit4(XOR("##V E bones"), (float*)&color::V_Bones_Enemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Visible Enemy"));
					ImGui::SameLine();
					ImGui::ColorEdit4(XOR("##NV E bones"), (float*)&color::NV_Bones_Enemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Not Visible Enemy"));
					ImGui::Text(XOR("2D Box"));
					ImGui::SameLine(125);
					ImGui::ColorEdit4(XOR("##V E 2dboxes"), (float*)&color::V_2dBoxes_Enemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Visible Enemy"));
					ImGui::SameLine();
					ImGui::ColorEdit4(XOR("##NV E 2dboxes"), (float*)&color::NV_2dBoxes_Enemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Not Visible Enemy"));
					ImGui::Text(XOR("Diamond"));
					ImGui::SameLine(125);
					ImGui::ColorEdit4(XOR("##V E 2ddboxes"), (float*)&color::V_2ddBoxes_Enemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Visible Enemy"));
					ImGui::SameLine();
					ImGui::ColorEdit4(XOR("##NV E 2ddboxes"), (float*)&color::NV_2ddBoxes_Enemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Not Visible Enemy"));
					ImGui::Text(XOR("Box Fill"));
					ImGui::SameLine(125);
					ImGui::ColorEdit4(XOR("##V 2dboxes bg E"), (float*)&color::V_2dBoxes_BG_Enemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Visible Enemy"));
					ImGui::SameLine();
					ImGui::ColorEdit4(XOR("##NV 2dboxes bg E"), (float*)&color::NV_2dBoxes_BG_Enemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Not Visible Enemy"));
					ImGui::Text(XOR("Self Revive"));
					ImGui::SameLine(125);
					ImGui::ColorEdit4(XOR("##V SR E"), (float*)&color::V_Bones_SREnemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Visible Enemy"));
					ImGui::SameLine();
					ImGui::ColorEdit4(XOR("##NV SR E"), (float*)&color::NV_Bones_SREnemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Not Visible Enemy"));
					ImGui::Text(XOR("Being Revived"));
					ImGui::SameLine(125);
					ImGui::ColorEdit4(XOR("##V PR E"), (float*)&color::V_Bones_BREnemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Visible Enemy"));
					ImGui::SameLine();
					ImGui::ColorEdit4(XOR("##NV PR E"), (float*)&color::NV_Bones_BREnemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Not Visible Enemy"));
					ImGui::Text(XOR("Downed"));
					ImGui::SameLine(125);
					ImGui::ColorEdit4(XOR("##V D E"), (float*)&color::V_Bones_DEnemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Visible Enemy"));
					ImGui::SameLine();
					ImGui::ColorEdit4(XOR("##NV D E"), (float*)&color::NV_Bones_DEnemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Not Visible Enemy"));
					ImGui::EndChild();

					ImGui::SetCursorPos(ImVec2(275, 60));
					ImGui::BeginChild(XOR("##colorchild1"), ImVec2(247, 340), true);
					ImGui::TextDisabled(XOR("Team"));
					ImGui::Separator();
					ImGui::ColorEdit4(XOR("##V F lines"), (float*)&color::V_Line_F, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Visible Friendly"));
					ImGui::SameLine();
					ImGui::ColorEdit4(XOR("##NV F lines"), (float*)&color::NV_Line_F, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Not Visible Friendly"));
					ImGui::ColorEdit4(XOR("##V F bones"), (float*)&color::V_Bones_F, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Visible Friendly"));
					ImGui::SameLine();
					ImGui::ColorEdit4(XOR("##NV F bones"), (float*)&color::NV_Bones_F, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Not Visible Friendly"));
					ImGui::ColorEdit4(XOR("##V F 2dboxes"), (float*)&color::V_2dBoxes_F, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Visible Friendly"));
					ImGui::SameLine();
					ImGui::ColorEdit4(XOR("##NV F 2dboxes"), (float*)&color::NV_2dBoxes_F, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Not Visible Friendly"));
					ImGui::ColorEdit4(XOR("##V F 2ddboxes"), (float*)&color::V_2ddBoxes_F, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Visible Friendly"));
					ImGui::SameLine();
					ImGui::ColorEdit4(XOR("##NV F 2ddboxes"), (float*)&color::NV_2ddBoxes_F, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Not Visible Friendly"));
					ImGui::ColorEdit4(XOR("##V 2dboxes bg F"), (float*)&color::V_2dBoxes_BG_F, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Visible Friendly"));
					ImGui::SameLine();
					ImGui::ColorEdit4(XOR("##NV 2dboxes bg F"), (float*)&color::NV_2dBoxes_BG_F, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Not Visible Friendly"));
					ImGui::ColorEdit4(XOR("##V SR F"), (float*)&color::V_Bones_SRF, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Visible Friendly"));
					ImGui::SameLine();
					ImGui::ColorEdit4(XOR("##NV SR F"), (float*)&color::NV_Bones_SRF, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Not Visible Friendly"));
					ImGui::ColorEdit4(XOR("##V PR F"), (float*)&color::V_Bones_BRF, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Visible Friendly"));
					ImGui::SameLine();
					ImGui::ColorEdit4(XOR("##NV PR F"), (float*)&color::NV_Bones_BRF, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Not Visible Friendly"));
					ImGui::ColorEdit4(XOR("##V D F"), (float*)&color::V_Bones_DF, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Visible Enemy"));
					ImGui::SameLine();
					ImGui::ColorEdit4(XOR("##NV D F"), (float*)&color::NV_Bones_DF, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Not Visible Enemy"));
					ImGui::EndChild();
					break;
				}

			}

			colors[ImGuiCol_ChildBg] = previousCBG;
			ImGui::EndChild();
			break;
		case 2:
			colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.12f, 0.12f, 0.67f);
			style->ChildBorderSize = 0.f;
			ImGui::SetCursorPos(ImVec2(70, 40));
			if (ImGui::BeginChild("##colorschildmain", ImVec2(530, 410), true))
			{
				ImGui::SetCursorPos(ImVec2(10, 60));
				ImGui::BeginChild(XOR("##miscchild0"), ImVec2(247, 340), true);
				ImGui::Checkbox(XOR("No Recoil"), &globals::b_recoil);
				ImGui::Checkbox(XOR("Warning System"), &globals::warning_system);
				ImGui::NewLine();
				ImGui::SetCursorPosX(20);
				render_SliderFloat("##WARNINGDISTANCE", globals::warning_dist, 200, 1.f, 300.f, customization::slider, "Distance: %.1fm");
				//ImGui::Checkbox(XOR("FFA Mode"), &globals::ffa);
				//if (ImGui::IsItemHovered())
					//ImGui::SetTooltip(XOR("Your shooting at your ''team mates'' (enemies) change team colors for esp"));
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
			}
			ImGui::EndChild();

			ImGui::SetCursorPos(ImVec2(275, 60));
			if (ImGui::BeginChild("##miscchild", ImVec2(247, 340), true))
			{
				ImGui::Checkbox(XOR("Snow Fall"), &globals::enableSnowfall);
				ImGui::Checkbox(XOR("Windows 7 AFK Circles"), &globals::enableCircles);
				ImGui::Checkbox(XOR("Game Border"), &globals::gameBorder);
				ImGui::SameLine();
				ImGui::ColorEdit4(XOR("##gameborder"), (float*)&customization::Gameborder, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
				ImGui::Checkbox(XOR("RGB Game Border"), &globals::rgbGB);

			}
			ImGui::EndChild();

			colors[ImGuiCol_ChildBg] = previousCBG;
			ImGui::EndChild();
			break;
		case 3:
			colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.12f, 0.12f, 0.67f);
			style->ChildBorderSize = 0.f;
			ImGui::SetCursorPos(ImVec2(70, 40));
			if (ImGui::BeginChild("##miscchildmain", ImVec2(530, 410), true))
			{
				// Calculate cursor position relative to child window
				ImVec2 childContentRegionMax = ImGui::GetWindowContentRegionMax();
				float xPosition = childContentRegionMax.x * 0.5f - 10;
				float yPosition = ImGui::GetCursorPosY() + 5;

				static std::string str_warn = "Config Must be at least 3 char length!";

				// Set cursor position
				ImGui::SetCursorPos(ImVec2(xPosition, yPosition));
				ImGui::Text(XOR("Config"));

				int i_config = 0;

				ImGui::Spacing();
				ImGui::Spacing();

				// -->

				ImGui::Text("Profile 1 | F1");
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Press F1 to Load this config.");

				ImGui::SameLine(80);

				if (ImGui::Button(XOR("Save Profile1")))
				{
					std::string buf = XOR("Profile_1");

					if (!vec_files.empty() || vec_files.empty())
						Settings::Save_Settings(buf);
				}

				ImGui::SameLine();

				if (ImGui::Button("Delete Profile1"))
				{
					if (!vec_files.empty())
					{
						std::string str_config = "C:\\Configs\\Profile_1.ini";
						if (remove(str_config.c_str()) != 0)
						{
							str_warn = "Could not delete!";
							ImGui::OpenPopup("warn");
						}
						str_config = "";
						Settings::str_config_name = "";
					}
				}
				//<--
				ImGui::NewLine();
				//-->
				ImGui::Text("Profile 2 | F2");
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Press F2 to Load this config.");

				ImGui::SameLine(80);

				if (ImGui::Button(XOR("Save Profile2")))
				{
					std::string profile2 = XOR("Profile_2");

					if (!vec_files.empty() || vec_files.empty())
						Settings::Save_Settings(profile2);
				}

				ImGui::SameLine();

				if (ImGui::Button("Delete Profile2"))
				{
					if (!vec_files.empty())
					{
						std::string str_config = "C:\\Configs\\Profile_2.ini";
						if (remove(str_config.c_str()) != 0)
						{
							str_warn = "Could not delete!";
							ImGui::OpenPopup("warn");
						}
						str_config = "";
						Settings::str_config_name = "";
					}
				}
				//<--
				ImGui::NewLine();
				//-->
				ImGui::Text("Profile 3 | F3");
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Press F3 to Load this config.");

				ImGui::SameLine(80);

				if (ImGui::Button(XOR("Save Profile3")))
				{
					std::string buf2 = XOR("Profile_3");

					if (!vec_files.empty() || vec_files.empty())
						Settings::Save_Settings(buf2);
				}

				ImGui::SameLine();

				if (ImGui::Button("Delete Profile3"))
				{
					if (!vec_files.empty())
					{
						std::string str_config = "C:\\Configs\\Profile_3.ini";
						if (remove(str_config.c_str()) != 0)
						{
							str_warn = "Could not delete!";
							ImGui::OpenPopup("warn");
						}
						str_config = "";
						Settings::str_config_name = "";
					}
				}
				//<--
			}

			colors[ImGuiCol_ChildBg] = previousCBG;
			ImGui::EndChild();
			break;
		case 4:
			colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.12f, 0.12f, 0.67f);
			style->ChildBorderSize = 0.f;
			ImGui::SetCursorPos(ImVec2(70, 40));
			if (ImGui::BeginChild("##miscchildmain", ImVec2(530, 410), true))
			{
				ImGui::Text(XOR("Text"));
				ImGui::SameLine(140);
				ImGui::ColorEdit4(XOR("##Text"), (float*)&customization::text, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

				ImGui::SameLine(180);
				ImGui::Text(XOR("Button"));
				ImGui::SameLine(270);
				ImGui::ColorEdit4(XOR("##BUTTON"), (float*)&customization::button, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

				ImGui::SameLine(420);
				if (ImGui::Button("Default Theme"))
				{
					customization::text = ImColor{ 255, 255, 255, 255 };
					customization::text_disabled = ImColor{ 128, 128, 128, 255 };
					customization::windowbg = ImColor{ 23, 18, 18, 255 };
					customization::popup = ImColor{ 20, 20, 20, 240 };
					customization::border = ImColor{ 255, 0, 0, 128 };
					customization::framebg = ImColor{ 49, 49, 49, 138 };
					customization::framebgh = ImColor{ 255, 84, 84, 102 };
					customization::framebga = ImColor{ 171, 15, 15, 171 };
					customization::checkmark = ImColor{ 255, 5, 5, 255 };
					customization::button = ImColor{ 31, 31, 31, 107 };
					customization::buttonh = ImColor{ 255, 84, 84, 102 };
					customization::buttona = ImColor{ 171, 15, 15, 171 };
					customization::slider = ImColor{ 255, 0, 0, 255 };
					customization::header = ImColor{ 92, 88, 88, 255 };
					customization::headerh = ImColor{ 255, 0, 0, 255 };
					customization::headera = ImColor{ 0, 255, 0, 255 };
					customization::wbs = 5.f;
					customization::wr = 0.f;
				}

				ImGui::Text(XOR("Text Disabled"));
				ImGui::SameLine(140);
				ImGui::ColorEdit4(XOR("##TextDisabled"), (float*)&customization::text_disabled, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

				ImGui::SameLine(180);
				ImGui::Text(XOR("Button Hover"));
				ImGui::SameLine(270);
				ImGui::ColorEdit4(XOR("##BUTTONHOVER"), (float*)&customization::buttonh, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

				ImGui::SameLine(420);
				if (ImGui::Button("Purple Theme"))
				{
					customization::text = ImColor{ 255, 255, 255, 255 };
					customization::text_disabled = ImColor{ 128, 128, 128, 255 };
					customization::windowbg = ImColor{ 23, 18, 18, 255 };
					customization::popup = ImColor{ 20, 20, 20, 240 };
					customization::border = ImColor{ 215, 21, 235, 128 };
					customization::framebg = ImColor{ 49, 49, 49, 138 };
					customization::framebgh = ImColor{ 170, 15, 223, 102 };
					customization::framebga = ImColor{ 232, 18, 235, 171 };
					customization::checkmark = ImColor{ 184, 14, 249, 255 };
					customization::button = ImColor{ 31, 31, 31, 107 };
					customization::buttonh = ImColor{ 166, 43, 221, 103 };
					customization::buttona = ImColor{ 232, 18, 235, 171 };
					customization::slider = ImColor{ 167, 11, 228, 255 };
					customization::header = ImColor{ 92, 88, 88, 255 };
					customization::headerh = ImColor{ 166, 43, 221, 103 };
					customization::headera = ImColor{ 232, 18, 235, 171 };
					customization::wbs = 3.6f;
					customization::wr = 0.f;
				}

				ImGui::Text(XOR("Window Background"));
				ImGui::SameLine(140);
				ImGui::ColorEdit4(XOR("##WindowBackground"), (float*)&customization::windowbg, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

				ImGui::SameLine(180);
				ImGui::Text(XOR("Button Active"));
				ImGui::SameLine(270);
				ImGui::ColorEdit4(XOR("##BUTTONACTIVE"), (float*)&customization::buttona, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

				ImGui::Text(XOR("Popup Background"));
				ImGui::SameLine(140);
				ImGui::ColorEdit4(XOR("##PopUPBackground"), (float*)&customization::popup, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

				ImGui::SameLine(180);
				ImGui::Text(XOR("Slider"));
				ImGui::SameLine(270);
				ImGui::ColorEdit4(XOR("##SLIDERFOAD"), (float*)&customization::slider, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

				ImGui::Text(XOR("Border"));
				ImGui::SameLine(140);
				ImGui::ColorEdit4(XOR("##Border"), (float*)&customization::border, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

				ImGui::SameLine(180);
				ImGui::Text(XOR("Header"));
				ImGui::SameLine(270);
				ImGui::ColorEdit4(XOR("##headerr"), (float*)&customization::header, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

				ImGui::Text(XOR("Frame Background"));
				ImGui::SameLine(140);
				ImGui::ColorEdit4(XOR("##FrameBackground"), (float*)&customization::framebg, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

				ImGui::SameLine(180);
				ImGui::Text(XOR("Header Hovered"));
				ImGui::SameLine(270);
				ImGui::ColorEdit4(XOR("##headerrh"), (float*)&customization::headerh, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

				ImGui::Text(XOR("Frame Background Hover"));
				ImGui::SameLine(140);
				ImGui::ColorEdit4(XOR("##FrameBackgroundHover"), (float*)&customization::framebgh, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

				ImGui::SameLine(180);
				ImGui::Text(XOR("Header Active"));
				ImGui::SameLine(270);
				ImGui::ColorEdit4(XOR("##headerra"), (float*)&customization::headera, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

				ImGui::Text(XOR("Frame Background Active"));
				ImGui::SameLine(140);
				ImGui::ColorEdit4(XOR("##FrameBackgroundActive"), (float*)&customization::framebga, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

				ImGui::Text(XOR("CheckMark"));
				ImGui::SameLine(140);
				ImGui::ColorEdit4(XOR("##CheckMark"), (float*)&customization::checkmark, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();

				render_SliderFloat("##WindowBordersize", customization::wbs, 125, 0, 20.0f, customization::slider, "Window Border Size: %.1f");

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();

				render_SliderFloat("##windowrounding", customization::wr, 125, 0, 20.0f, customization::slider, "Window Rounding: %.1f");

				ImGui::Spacing();
			}

			colors[ImGuiCol_ChildBg] = previousCBG;
			ImGui::EndChild();
			break;
		}

		ImGui::SetCursorPos(ImVec2(10, 10));
		ImGui::TextDisabled("DNS DUMP");
		//ImGui::Text("Controller Connected: %s", aimbot::isControllerConnected() ? "Yes" : "No");

		ImGui::SetCursorPos(ImVec2(475, 10));
		EditorColorScheme::MySwitch(XOR("Cheats Enabled"), &globals::menu_available);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip(XOR("Self Explanatory Hit End on keyboard to enable/disable Cheats"));

		ImGui::End();
	}

}

void init(ImFont* font)
{
	auto draw = ImGui::GetForegroundDrawList();

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::Begin("A", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
	ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);

	globals::b_in_game = sdk::in_game();

	if (!globals::b_in_game && screenshot::visuals)
	{
		g_game::e_data::refdef = nullptr;
		globals::menu_available = false;
		globals::max_player_count = 0;
	}

	if (globals::b_in_game && screenshot::visuals)
	{
		aimbot::start_tick();

		if (g_game::e_data::refdef == nullptr)
			g_game::e_data::refdef = sdk::get_refdef();

		if (g_game::e_data::refdef == nullptr) return;
		globals::refdefadd = g_game::e_data::refdef;

		globals::max_player_count = sdk::get_max_player_count();

		if (!globals::is_aiming)
			aimbot::reset_lock();

		if (globals::b_recoil)
		{
			if (globals::platform == 0)
			{
				uintptr_t client_info = bnet::get_client_info();
				if (is_bad_ptr(client_info))return;

				__int64 r12 = client_info;
				r12 += bnet::recoil;
				__int64 rsi = r12 + 0x4;
				DWORD edx = *(uint64_t*)(r12 + 0xC);
				DWORD ecx = (DWORD)r12;
				ecx ^= edx;
				DWORD eax = (DWORD)((__int64)ecx + 0x2);
				eax *= ecx;
				ecx = (DWORD)rsi;
				ecx ^= edx;
				DWORD udZero = eax;
				//left, right
				eax = (DWORD)((__int64)ecx + 0x2);
				eax *= ecx;
				DWORD lrZero = eax;
				(DWORD)((uint64_t)r12, udZero, sizeof(udZero));
			}
			else if (globals::platform == 1)
			{
				uintptr_t client_info = steam::Get_cg_t();
				if (is_bad_ptr(client_info))return;

				__int64 r12 = client_info;
				r12 += steam::recoil;
				__int64 rsi = r12 + 0x4;
				DWORD edx = *(uint64_t*)(r12 + 0xC);
				DWORD ecx = (DWORD)r12;
				ecx ^= edx;
				DWORD eax = (DWORD)((__int64)ecx + 0x2);
				eax *= ecx;
				ecx = (DWORD)rsi;
				ecx ^= edx;
				DWORD udZero = eax;
				//left, right
				eax = (DWORD)((__int64)ecx + 0x2);
				eax *= ecx;
				DWORD lrZero = eax;
				(DWORD)((uint64_t)r12, udZero, sizeof(udZero));
			}
		}

		if (globals::b_crosshair)
		{
			ImVec2 center = ImVec2(g_game::e_data::refdef->Width / 2, g_game::e_data::refdef->Height / 2);

			float radius = globals::b_crosshair_size;
			int numSegments = 125; // Number of line segments used to approximate the circle

			float size = globals::b_crosshair_size;
			float halfSize = size / 2.0f;
			float quarterSize = size / 4.0f;

			switch (globals::b_crosshair_type)
			{
			case 0:
			{
				//cross
				esp::g_draw::draw_line(ImVec2((center.x), (center.y) - globals::b_crosshair_size), ImVec2((center.x), (center.y) + globals::b_crosshair_size), color::crosshair, globals::b_crosshair_thickness);
				esp::g_draw::draw_line(ImVec2((center.x) - globals::b_crosshair_size, (center.y)), ImVec2((center.x) + globals::b_crosshair_size, (center.y)), color::crosshair, globals::b_crosshair_thickness);
				break;
			}
			case 1:
			{
				//Dot
				draw->AddCircleFilled(center, globals::b_crosshair_size, color::crosshair, numSegments);
				break;
			}
			case 2:
			{
				//circle
				draw->AddCircle(center, radius, color::crosshair, numSegments, globals::b_crosshair_thickness);
				break;
			}
			case 3:
			{
				//smiley face
				draw->AddCircle(center, radius, color::crosshair, numSegments, globals::b_crosshair_thickness);

				// Calculate the position of the eyes and mouth
				float eyeOffset = radius * 0.2f;
				ImVec2 leftEyePos(center.x - eyeOffset, center.y - eyeOffset);
				ImVec2 rightEyePos(center.x + eyeOffset, center.y - eyeOffset);
				ImVec2 mouthPos(center.x, center.y + eyeOffset * 0.8f);
				float mouthRadius = radius * 0.5f;

				// Draw the eyes
				float eyeRadius = radius * 0.1f;
				draw->AddCircleFilled(leftEyePos, eyeRadius, color::crosshair);
				draw->AddCircleFilled(rightEyePos, eyeRadius, color::crosshair);

				// Draw the mouth
				float smileStartAngle = 0.1f * 3.14159f; // 0.1 is an arbitrary value to control the smile angle
				float smileEndAngle = 0.9f * 3.14159f; // 0.9 is an arbitrary value to control the smile angle
				draw->PathArcTo(center, mouthRadius, smileStartAngle, smileEndAngle, numSegments);
				draw->PathStroke(color::crosshair, false, globals::b_crosshair_thickness);
				break;
			}
			case 4:
			{
				//diamond
				ImVec2 topPoint(center.x, center.y - halfSize);
				ImVec2 rightPoint(center.x + halfSize, center.y);
				ImVec2 bottomPoint(center.x, center.y + halfSize);
				ImVec2 leftPoint(center.x - halfSize, center.y);

				// Draw the lines to form the diamond shape
				draw->AddLine(topPoint, rightPoint, color::crosshair, globals::b_crosshair_thickness);
				draw->AddLine(rightPoint, bottomPoint, color::crosshair, globals::b_crosshair_thickness);
				draw->AddLine(bottomPoint, leftPoint, color::crosshair, globals::b_crosshair_thickness);
				draw->AddLine(leftPoint, topPoint, color::crosshair, globals::b_crosshair_thickness);
				break;
			}

			}
		}

		if (globals::b_fov)
			esp::g_draw::draw_fov(globals::f_fov_size);

		for (int i = 0; i < globals::max_player_count; ++i)
		{                                                                          
			sdk::player_t local_player = sdk::get_local_player();
			sdk::player_t player_info = sdk::get_player(i);

			if (is_bad_ptr(local_player.address) || is_bad_ptr(player_info.address))continue;
			if (local_player.get_index() == i)continue;

			//sdk::clientinfo_t* clientinfo = reinterpret_cast<sdk::clientinfo_t*>(sdk::GetClientInfo(i));
			//if (is_bad_ptr(clientinfo))
			//	continue;

			Vector2 ScreenPos, headpos2d, bone_screen_pos;
			ImColor box_color, box_bg_color, d_color, line_color, bones_color;
			Vector3 dpos = player_info.get_pos();
			Vector3 mpos = local_player.get_pos();
			Vector3 current_bone;
			Vector3 Head_pos = dpos;
			Head_pos.z += 58.0;

			auto player_health = sdk::get_player_health(i);
			float healthPercentage = ((float)player_health / 100.0f) * 100.0f;

			auto is_dead = player_info.is_dead() && healthPercentage <= 0;
			auto is_friendly = player_info.team_id() == local_player.team_id();
			auto is_valid = player_info.is_valid();
			auto fdistance = g_game::units_to_m(local_player.get_pos().distance_to(player_info.get_pos()));
			auto visible = g_game::is_visible(i);

			if (is_friendly && !globals::b_friendly)continue;

			if (!visible && globals::vis_only)continue;

			if (fdistance >= globals::max_distance)continue;

			if (is_dead)continue;

			if (!aimbot::check_valid_bones(i))continue;

			if (!esp::w2s(dpos, &ScreenPos))continue;
			if (!esp::w2s(Head_pos, &headpos2d))continue;

			float PawnHeight = abs(headpos2d.y - ScreenPos.y);
			float PawnWidth = PawnHeight * 0.55f;

			if (!globals::b_visible && visible || globals::b_visible && visible)
			{
				box_color = is_friendly ? color::V_2dBoxes_F : color::V_2dBoxes_Enemy;
				box_bg_color = is_friendly ? color::V_2dBoxes_BG_F : color::V_2dBoxes_BG_Enemy;
				d_color = is_friendly ? color::V_2ddBoxes_F : color::V_2ddBoxes_Enemy;
				line_color = is_friendly ? color::V_Line_F : color::V_Line_Enemy;
				bones_color = is_friendly ? color::V_Bones_F : color::V_Bones_Enemy;
			}
			else
			{
				box_color = is_friendly ? color::NV_2dBoxes_F : color::NV_2dBoxes_Enemy;
				box_bg_color = is_friendly ? color::NV_2dBoxes_BG_F : color::NV_2dBoxes_BG_Enemy;
				d_color = is_friendly ? color::NV_2ddBoxes_F : color::NV_2ddBoxes_Enemy;
				line_color = is_friendly ? color::NV_Line_F : color::NV_Line_Enemy;
				bones_color = is_friendly ? color::NV_Bones_F : color::NV_Bones_Enemy;
			}

			//2d box
			if (globals::b_box)
			{
				//// Draw background
				if (globals::box_bg) {
					auto drawList = ImGui::GetBackgroundDrawList();
					// get height and width from the first draw line
					float height = headpos2d.y + PawnHeight - (headpos2d.y - 6);
					float width = headpos2d.x + (PawnWidth / 2) + 3 - (headpos2d.x - (PawnWidth / 2) - 3);

					// draw the filled rectangle using the same dimensions
					drawList->AddRectFilled(ImVec2(headpos2d.x - (PawnWidth / 2) - 3, headpos2d.y - 6), ImVec2(headpos2d.x - (PawnWidth / 2) - 3 + width, headpos2d.y - 6 + height), box_bg_color);
				}

				draw->AddLine(ImVec2(headpos2d.x - (PawnWidth / 2) - 3, headpos2d.y - 6), ImVec2(headpos2d.x + (PawnWidth / 2) + 3, headpos2d.y - 6), box_color, globals::box_thickness);
				draw->AddLine(ImVec2(headpos2d.x - (PawnWidth / 2) - 3, headpos2d.y - 6), ImVec2(headpos2d.x - (PawnWidth / 2) - 3, headpos2d.y + PawnHeight), box_color, globals::box_thickness);
				draw->AddLine(ImVec2(headpos2d.x + (PawnWidth / 2) + 3, headpos2d.y - 6), ImVec2(headpos2d.x + (PawnWidth / 2) + 3, headpos2d.y + PawnHeight), box_color, globals::box_thickness);
				draw->AddLine(ImVec2(headpos2d.x - (PawnWidth / 2) - 3, headpos2d.y + PawnHeight), ImVec2(headpos2d.x + (PawnWidth / 2) + 3, headpos2d.y + PawnHeight), box_color, globals::box_thickness);
			}

			//diamond box
			if (globals::esp_box)
			{
				// Calculate the center of the diamond using the provided screen position and head position
				ImVec2 center = ImVec2(ScreenPos.x, headpos2d.y);

				// Calculate half of the width and height of the diamond using the provided values
				float half_width = PawnWidth * 0.55f;
				float half_height = PawnHeight * 0.55f;

				// Calculate the four corners of the diamond based on the center and dimensions
				ImVec2 top = ImVec2(center.x, center.y - half_height);
				ImVec2 right = ImVec2(center.x + half_width, center.y);
				ImVec2 bottom = ImVec2(center.x, center.y + half_height);
				ImVec2 left = ImVec2(center.x - half_width, center.y);

				// Add the two triangles that make up the diamond to the draw list
				draw->AddLine(top, right, d_color, globals::box_thickness);
				draw->AddLine(right, bottom, d_color, globals::box_thickness);
				draw->AddLine(bottom, left, d_color, globals::box_thickness);
				draw->AddLine(left, top, d_color, globals::box_thickness);
			}

			//snap lines
			if (globals::b_line)
			{
				switch (globals::linetype)
				{
				case 1://shows lines in middle of screen
					esp::g_draw::draw_line(ImVec2(g_game::e_data::refdef->Width / 2, g_game::e_data::refdef->Height), ImVec2(ScreenPos.x, ScreenPos.y), line_color, globals::lines_thickness);
					break;
				case 2://shows lines in bottom of screen
					esp::g_draw::draw_line(ImVec2(g_game::e_data::refdef->Width / 2, g_game::e_data::refdef->Height / 2), ImVec2(ScreenPos.x, ScreenPos.y), line_color, globals::lines_thickness);
					break;
				}
			}

			//Perfect Skeleton (bones)
			if (globals::b_skeleton)
			{
				//if (clientinfo->game_extrainfo & sdk::BRDataFlags::Downed)
				//{
				//	if (!globals::b_visible && visible || globals::b_visible && visible)
				//		bones_color = is_friendly ? color::V_Bones_DF : color::V_Bones_DEnemy;
				//	else
				//		bones_color = is_friendly ? color::NV_Bones_DF : color::NV_Bones_DEnemy;
				//}

				//if (clientinfo->game_extrainfo & sdk::BRDataFlags::SelfReviving)
				//{
				//	if (!globals::b_visible && visible || globals::b_visible && visible)
				//		bones_color = is_friendly ? color::V_Bones_SRF : color::V_Bones_SREnemy;
				//	else
				//		bones_color = is_friendly ? color::NV_Bones_SRF : color::NV_Bones_SREnemy;
				//}

				//if (clientinfo->game_extrainfo & sdk::BRDataFlags::BeingRevived)
				//{
				//	if (!globals::b_visible && visible || globals::b_visible && visible)
				//		bones_color = is_friendly ? color::V_Bones_BRF : color::V_Bones_BREnemy;
				//	else
				//		bones_color = is_friendly ? color::NV_Bones_BRF : color::NV_Bones_BREnemy;
				//}

				//draw
				//pelvis to neck
				esp::d_bones(aimbot::header_to_bladder[5], aimbot::header_to_bladder[1], dpos, bones_color);
				//pelvis to left foot 2
				esp::d_bones(aimbot::header_to_bladder[5], aimbot::left_foot_to_bladder[2], dpos, bones_color);
				//left foot 2 to left foot 3
				esp::d_bones(aimbot::left_foot_to_bladder[2], aimbot::left_foot_to_bladder[3], dpos, bones_color);
				//pelvis to right foot 2
				esp::d_bones(aimbot::header_to_bladder[5], aimbot::right_foot_to_bladder[2], dpos, bones_color);
				//right foot 2 to right foot 3
				esp::d_bones(aimbot::right_foot_to_bladder[2], aimbot::right_foot_to_bladder[3], dpos, bones_color);
				//neck to right hand 2
				esp::d_bones(aimbot::header_to_bladder[1], aimbot::left_hand[2], dpos, bones_color);
				//right hand 2 to right hand 3
				esp::d_bones(aimbot::left_hand[2], aimbot::left_hand[3], dpos, bones_color);
				//right hand 3 to right hand 4
				esp::d_bones(aimbot::left_hand[3], aimbot::left_hand[4], dpos, bones_color);
				//neck to left hand 2
				esp::d_bones(aimbot::header_to_bladder[1], aimbot::right_hand[2], dpos, bones_color);
				//left hand 2 to left hand 3
				esp::d_bones(aimbot::right_hand[2], aimbot::right_hand[3], dpos, bones_color);
				//left hand 3 to left hand 4
				esp::d_bones(aimbot::right_hand[3], aimbot::right_hand[4], dpos, bones_color);
			}

			//healthbar
			if (globals::draw_health)
			{
				auto draw = ImGui::GetBackgroundDrawList();
				auto pos = player_info.get_pos();

				Vector2 bottom, top;

				if (!esp::wts(pos, &bottom))
					return;

				pos.z += 50;
				if (!esp::wts(pos, &top))
					return;

				top.y -= 5;
				auto height = top.y - bottom.y;
				auto width = height / 2.f;
				auto y = bottom.y;
				auto bar_width = max(0, min(127, healthPercentage)) * (height - 5) / 127.f;
				uint32_t color;

				if (healthPercentage >= 70.0f)
				{
					color = ImGui::ColorConvertFloat4ToU32(ImColor(37, 219, 43, 255));
				}
				else if (healthPercentage >= 40.0f)
				{
					color = ImGui::ColorConvertFloat4ToU32(ImColor(245, 180, 39, 255));
				}
				else
				{
					color = ImGui::ColorConvertFloat4ToU32(ImColor(214, 34, 21, 255));
				}

				if (!globals::b_box)
				{
					auto x = top.x + 2 - width / 1.8f;

					draw->AddRectFilled(ImVec2(x, y), ImVec2(x + 3, y + bar_width), color);
					draw->AddLine({ x, y }, { x, y + bar_width }, color);
					draw->AddLine({ x, y }, { x + 3, y }, color);
					draw->AddLine({ x, y + bar_width }, { x + 3, y + bar_width }, color);
				}
				else if (globals::b_box)
				{
					auto x = top.x + 5 - width / 1.8f;

					draw->AddRectFilled(ImVec2(x, y), ImVec2(x + 3, y + bar_width), color);
					draw->AddLine({ x, y }, { x, y + bar_width }, color);
					draw->AddLine({ x, y }, { x + 3, y }, color);
					draw->AddLine({ x, y + bar_width }, { x + 3, y + bar_width }, color);
				}
			}

			//Names/Distance/Stance/Health display
			if (globals::b_namee)
			{
				char name_distance_teamid[128];
				sprintf(name_distance_teamid, "%d|%s [%0.1fm]", player_info.team_id(), sdk::get_player_name(i), fdistance);
				esp::g_draw::draw_sketch_edge_text(font, name_distance_teamid, ImVec2(ScreenPos.x, ScreenPos.y), globals::esp_text_size, ImColor(255, 255, 255, 255), true);
			}

			if (globals::b_lock)
			{
				if (!is_friendly && is_valid)
				{
					if (globals::bone_scan)
					{
						aimbot::update_vel_map(i, player_info.get_pos());
						aimbot::update_vel_map(aimbot::local_index(), local_player.get_pos());

						if (sdk::get_bone_by_player_index(i, aimbot::get_bone_opt(), &current_bone))
						{
							if (!esp::w2s(current_bone, &bone_screen_pos))continue;

							if (fdistance < globals::f_min_closest)
							{
								if (globals::prediction)
									current_bone = aimbot::prediction_solver(local_player.get_pos(), player_info.get_pos(), i, globals::bullet_speed);

								aimbot::update_vel_map(i, current_bone);

								if (!globals::b_visible && visible || globals::b_visible)
								{
									g_game::aimbot(bone_screen_pos, fdistance, healthPercentage, visible);
								}
							}
						}
					}
				}
			}
		}
		aimbot::is_aiming();
	}
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	window->DrawList->PushClipRectFullScreen();
	ImGui::End();
	ImGui::PopStyleColor();
}

void hookExecuteCommandListsD3D12(ID3D12CommandQueue* queue, UINT NumCommandLists, ID3D12CommandList* ppCommandLists)
{
	if (!d3d12test::d3d12CommandQueue2)
		d3d12test::d3d12CommandQueue2 = queue;

	oExecuteCommandListsD3D12(queue, NumCommandLists, ppCommandLists);
}
__declspec(dllexport)HRESULT present_hk(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!pSwapChain || !screenshot::visuals)
		return ori_present(pSwapChain, SyncInterval, Flags);

	d3d12test::d3d12CommandQueue2 = *reinterpret_cast<decltype(d3d12test::d3d12CommandQueue2)*>((std::uintptr_t)pSwapChain + 0x118);

	ImFont* main_font = imgui::begin_imgui(static_cast<IDXGISwapChain3*>(pSwapChain), d3d12test::d3d12CommandQueue2, nullptr);

	if (!screenshot::visuals)
		return ori_present(pSwapChain, SyncInterval, Flags);

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	menu();

	if (Settings::GetList().empty())
		CreateDirectoryA(XOR("C:\\Configs"), NULL);

	if (!globals::gameBorder)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowBorderSize = 0.f;
	}
	if (globals::gameBorder)
	{
		if (!globals::rgbGB)
		{
			ImVec4* colors = ImGui::GetStyle().Colors;
			ImGuiStyle& style = ImGui::GetStyle();
			colors[ImGuiCol_Border] = customization::Gameborder;
			style.WindowBorderSize = 5.f;
		}
		else if (globals::rgbGB)
		{
			static float rainbow;
			rainbow += 0.0010f; //0.0001f
			if (rainbow > 1.f)
				rainbow = 0.f;

			auto rgb = ImColor::HSV(rainbow, 1.f, 1.f, 0.8f);     // Rainbow Items

			ImVec4* colors = ImGui::GetStyle().Colors;
			ImGuiStyle& style = ImGui::GetStyle();
			colors[ImGuiCol_Border] = rgb;
			style.WindowBorderSize = 5.f;
		}
	}
	init(main_font);

	FrameContext* frameCtxt = imgui::WaitForNextFrameResources();
	UINT backBufferIdx = d3d12::pSwapChain->GetCurrentBackBufferIndex();
	{
		frameCtxt->CommandAllocator->Reset();
		static D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.pResource = d3d12::pID3D12ResourceArray[backBufferIdx];
		d3d12::pCommandList->Reset(frameCtxt->CommandAllocator, NULL);
		d3d12::pCommandList->ResourceBarrier(1, &barrier);
		d3d12::pCommandList->OMSetRenderTargets(1, &d3d12::RenderTargetDescriptorArray[backBufferIdx], FALSE, NULL);
		d3d12::pCommandList->SetDescriptorHeaps(1, &d3d12::pSrvDescHeap);
	}

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), d3d12::pCommandList);

	static D3D12_RESOURCE_BARRIER barrier = { };
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.pResource = d3d12::pID3D12ResourceArray[backBufferIdx];

	d3d12::pCommandList->ResourceBarrier(1, &barrier);
	d3d12::pCommandList->Close();

	d3d12::pCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&d3d12::pCommandList);

	//HRESULT results = ori_present(pSwapChain, SyncInterval, Flags);

	UINT64 fenceValue = d3d12::fenceLastSignaledValue + 1;
	d3d12::pCommandQueue->Signal(d3d12::pFence, fenceValue);
	d3d12::fenceLastSignaledValue = fenceValue;
	frameCtxt->FenceValue = fenceValue;

	return  ori_present(pSwapChain, SyncInterval, Flags);
}

VOID initialize()
{
	g_data::init();
	if (kiero::init(kiero::RenderType::D3D12) == kiero::Status::Success)
	{
		kiero::bind(140, (void**)&ori_present, present_hk);
	}
}

static uintptr_t __cdecl I_beginthreadex(
	void* _Security,
	unsigned                 _StackSize,
	_beginthreadex_proc_type _StartAddress,
	void* _ArgList,
	unsigned                 _InitFlag,
	unsigned* _ThrdAddr) {

	return iat(_beginthreadex).get()(_Security, _StackSize, _StartAddress, _ArgList, _InitFlag, _ThrdAddr);
}

BOOL APIENTRY DllMain(HMODULE module, DWORD rrr, LPVOID reserved)
{
	if (rrr == DLL_PROCESS_ATTACH)
	{
		I_beginthreadex(0, 0, (_beginthreadex_proc_type)initialize, 0, 0, 0);
	}
	return TRUE;
}

namespace ogr_function
{
	tWndProc WndProc;
}

bool isImGuiOverlayVisible = true;

void HideImGuiOverlay()
{
	isImGuiOverlayVisible = false;
}

void ShowImGuiOverlay()
{
	isImGuiOverlayVisible = true;
}

LRESULT hkWndProc(HWND hWnd, UINT Msg, WPARAM wp, LPARAM lp)
{
	switch (Msg)
	{
	case WM_SIZE:
	{
		if (Msg == WM_SIZE && wp == SIZE_MINIMIZED)
		{
			// Window is minimized, hide the ImGui overlay
			HideImGuiOverlay();
			break;
		}

		// Your existing code for WM_SIZE goes here
		// ...
		if (Msg == WM_SIZE && wp == SIZE_MINIMIZED)
			break;

		if (imgui::IsReady())
		{

			imgui::clear();

			ImGui_ImplDX12_InvalidateDeviceObjects();


			imgui::reset_imgui_request();
		}
		break;
	}
	}

	if (isImGuiOverlayVisible)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wp, lp);
		bool blockInput = io.WantCaptureMouse;

		if (blockInput && (Msg == WM_LBUTTONDOWN || Msg == WM_LBUTTONUP || Msg == WM_RBUTTONDOWN || Msg == WM_RBUTTONUP || Msg == WM_MBUTTONDOWN || Msg == WM_MBUTTONUP || Msg == WM_MOUSEWHEEL || Msg == WM_MOUSEMOVE))
		{
			if (globals::b_menu_open == true)
				return TRUE;
		}
	}

	return ogr_function::WndProc(hWnd, Msg, wp, lp);
}

void WndProc_hk()
{
	ogr_function::WndProc = (WNDPROC)SetWindowLongPtrW(g_data::hWind, GWLP_WNDPROC, (LONG_PTR)hkWndProc);
}