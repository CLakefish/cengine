#include "InputManager.h"
#include <ctype.h>

static void Input_AddStandard(InputManager* inputs) {
	Input_AddBind(inputs, "mouse", GLFW_MOUSE_BUTTON_1, IT_MOUSE);
	Input_AddBind(inputs, "mousePos", -1, IT_MOUSEVEC);
	Input_AddBind(inputs, "escape", GLFW_KEY_ESCAPE, IT_KEY);
	Input_AddBind(inputs, "W", GLFW_KEY_W, IT_KEY);
	Input_AddBind(inputs, "A", GLFW_KEY_A, IT_KEY);
	Input_AddBind(inputs, "S", GLFW_KEY_S, IT_KEY);
	Input_AddBind(inputs, "D", GLFW_KEY_D, IT_KEY);
	Input_AddBind(inputs, "space", GLFW_KEY_SPACE, IT_KEY);
	Input_AddBind(inputs, "leftShift", GLFW_KEY_LEFT_SHIFT, IT_KEY);
	Input_AddBind(inputs, "tab", GLFW_KEY_TAB, IT_KEY);
}

static int strnccmp(const char* a, const char* b) {
	while (*b) {
		int d = tolower(*a) - tolower(*b);
		if (d) {
			return d;
		}

		a++;
		b++;
	}

	return tolower(*a);
}

#pragma region Serialization

static void Input_Serialize(void* ctx) {
	InputManager* m = (InputManager*)ctx;

	fwrite(&m->count, sizeof(int), 1, m->serializer->file);

	for (int i = 0; i < m->count; ++i) {
		input_t t = m->inputs[i];
		size_t len = strlen(t.name);

		fwrite(&len, sizeof(size_t), 1, m->serializer->file);
		fwrite(t.name, len, 1, m->serializer->file);
		fwrite(&t.keyCode, sizeof(int), 1, m->serializer->file);
		fwrite(&t.type, sizeof(input_type), 1, m->serializer->file);
		//fprintf(file, "%s, %d, %d\n", t.name, t.keyCode, t.type);
	}
}

static void Input_Deserialize(void* ctx) {
	InputManager* m = (InputManager*)ctx;

	int count = -1;
	fread(&count, sizeof(count), 1, m->serializer->file);

	if (m->count <= 0)
	{
		m->count = 0;
		m->capacity = count;
		m->inputs = (input_t*)calloc(count, sizeof(input_t));

		if (!m->inputs) {
			perror("calloc");
			return;
		}
	}

	int keyCode = -1;
	int type = 0;
	size_t len = -1;

	for (int i = 0; i < count; ++i) {
		fread(&len, sizeof(size_t), 1, m->serializer->file);

		char* str = (char*)calloc(1, (size_t)(len + 1));

		if (!str) {
			return;
		}

		fread(str, len * sizeof(char), 1, m->serializer->file);
		fread(&keyCode, sizeof(int), 1, m->serializer->file);
		fread(&type, sizeof(int), 1, m->serializer->file);

		Input_AddBind(m, str, keyCode, type);
		free(str);
	}
}

#pragma endregion

#pragma region Addition_and_Removal

static void Input_Expand(InputManager* m) {
	m->capacity *= 2;
	input_t* t = realloc(m->inputs, m->capacity * sizeof(input_t));
	if (!t) {
		perror("Could not realloc input manager!");
		return;
	}

	m->inputs = t;
}

input_t* Input_AddBind(InputManager* m, char* name, int keyCode, input_type type) {
	for (int i = 0; i < m->count; ++i) {
		if (strnccmp(m->inputs[i].name, name) == 0) {
			printf("Already contains keycode of type %d with name '%s'\n", keyCode, name);
			return NULL;
		}
	}

	input_t tmp = { 0 };
	tmp.keyCode = keyCode;
	tmp.type = type;

	size_t len = strlen(name);
	char* buf = calloc(len + 1, sizeof * buf);

	if (!buf) {
		perror("Fhuc");
		return NULL;
	}

	memcpy(buf, name, len);
	tmp.name = buf;

	if (type == IT_KEY || type == IT_MOUSE) {
		tmp.data.iBool = (inputBool_t){ 0 };
	}
	else {
		tmp.data.iVec = (inputVec2_t){ 0 };
	}

	if (m->count >= m->capacity) {
		Input_Expand(m);
	}

	m->inputs[m->count] = tmp;
	return &m->inputs[m->count++];
}

void Input_RemoveBind(InputManager* m, int keyCode) {
	input_t* temp = Input_KeycodeInput(m, keyCode);

	if (temp == NULL) {
		printf("InputManager does not contain keycode of type %d\n", keyCode);
		return;
	}

	// TO ADD

	free(temp->name);
}

#pragma endregion

#pragma region Events

void Input_CalculateEvents(GLFWwindow* w, InputManager* m) {
	for (int i = 0; i < m->count; ++i) {
		input_t* data = &m->inputs[i];
		if (data->disabled) continue;
		Input_ForceCheck(w, data);
	}
}

void Input_ForceCheck(GLFWwindow* w, input_t* input) {
	switch (input->type) {
	case IT_MOUSEVEC: {
		inputVec2_t* data = &input->data.iVec;

		double x, y;
		data->prevX = data->currX;
		data->prevY = data->currY;
		glfwGetCursorPos(w, &x, &y);
		data->currX = x;
		data->currY = y;
		break;
	}

	default: {
		inputBool_t* data = &input->data.iBool;
		int holding = 0;

		if (input->type == IT_MOUSE) {
			holding = glfwGetMouseButton(w, input->keyCode);
		}
		else {
			holding = glfwGetKey(w, input->keyCode);
		}

		if (holding == GLFW_PRESS) {
			if (data->pressed) {
				data->held = 1;
				data->pressed = 0;
			}
			else if (!data->held) {
				data->pressed = 1;
			}

			return;
		}

		data->released = data->held ? 1 : 0;
		data->pressed = data->held = 0;
		break;
	}
	}
}

#pragma endregion

InputManager* Input_Init(void) {
	InputManager* m = (InputManager*)calloc(1, sizeof(InputManager));

	if (m == NULL) {
		perror("Could not allocate input manager!");
		return NULL;
	}

	m->capacity = 1;
	m->count    = 0;

	m->inputs = (input_t*)calloc(1, sizeof(input_t));

	if (m->inputs == NULL) {
		perror("Could not allocate input manager!");
		free(m);
		return NULL;
	}

	m->serializer = Serializer_Init("Inputs.srv", (void*)m);
	Serializer_Register(m->serializer, "Keyboard Inputs", Input_Serialize, Input_Deserialize);

	return m;
}

void Input_Shutdown(InputManager* m) {
	Serializer_Destroy(m->serializer);

	for (int i = 0; i < m->count; ++i) {
		free(m->inputs[i].name);
	}

	free(m->inputs);
	free(m);
}

void Input_Save(InputManager* m) {
	Serializer_Write(m->serializer);
}

void Input_Load(InputManager* m) {
	Input_AddStandard(m);
	Serializer_Load(m->serializer);
}

input_t* Input_KeycodeInput(InputManager* m, int keyCode) {
	if (keyCode == -1) return NULL;

	for (int i = 0; i < m->count; ++i) {
		if (m->inputs[i].keyCode == keyCode) {
			return &m->inputs[i];
		}
	}

	return NULL;
}

input_t* Input_NamedInput(InputManager* m, const char* name) {
	for (int i = 0; i < m->count; ++i) {
		if (strnccmp(m->inputs[i].name, name) == 0) {
			return &m->inputs[i];
		}
	}

	return NULL;
}

inputBool_t Input_Bool(InputManager* m, const char* name) {
	return Input_NamedInput(m, name)->data.iBool;
}

inputVec2_t Input_Vec(InputManager* m, const char* name) {
	return Input_NamedInput(m, name)->data.iVec;
}