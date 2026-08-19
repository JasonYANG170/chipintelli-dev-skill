使用cJSON

**1、include “cJSON.h"**

**2、数据结构**

cJSON结构体数据类型：

```c
/* The cJSON structure: */

typedef struct cJSON

{
    struct cJSON *next;

    struct cJSON *prev;

    struct cJSON *child;

    int type;

    char *valuestring;

    /* writing to valueint is DEPRECATED, use cJSON_SetNumberValue instead */

    int valueint;

    double valuedouble;

    char *string;
} cJSON;
```

这种类型的项表示一个JSON值。类型存储在type中作为位标志(这意味着您不能通过比较类型的值来查找类型)。

要检查项的类型，请使用对应的cJSON_Is…函数。它执行NULL检查，然后执行类型检查，如果项是这种类型，则返回一个布尔值。

类型可以是下列类型之一:

cJSON_Invalid(用cJSON_IsInvalid检查):表示一个无效的不包含任何值的项。如果将项设置为所有零字节，则自动具有此类型。

cJSON_False(用cJSON_IsFalse检查):表示一个错误的布尔值。您还可以使用cJSON_IsBool检查布尔值。

cJSON_True(用cJSON_IsTrue检查):表示一个真实的布尔值。您还可以使用cJSON_IsBool检查布尔值。

cJSON_NULL(检查cJSON_IsNull):表示一个空值。

cJSON_Number(检查cJSON_IsNumber):表示一个数字值。该值在valuedouble和valueint中以double形式存储。如果数值超出了整数的范围，则valueint使用INT_MAX或INT_MIN。

cJSON_String:表示一个字符串值。它以零终止字符串的形式存储在valuestring中。

cJSON_Array(检查cJSON_IsArray):表示一个数组值。这是通过将child指向表示数组中值的cJSON项链表来实现的。元素使用next和prev链接在一起，其中第一个元素是prev。next == NULL和最后一个元素next == NULL。

cJSON_Object(用cJSON_IsObject检查):表示一个对象值。对象的存储方式与数组相同，唯一的区别是对象中的项将它们的键存储为string。

cJSON_Raw(检查cJSON_IsRaw):表示任何类型的JSON，它存储在valuestring中以零结尾的字符数组中。例如，可以使用它来避免反复打印相同的静态JSON以节省性能。cJSON在解析时永远不会创建这种类型。还要注意，cJSON不检查它是否是有效的JSON。

另外还有以下两个标志:

cJSON_IsReference:指定子元素所指向的项和/或valuestring不属于这个项，它只是一个引用。所以cJSON_Delete和其他函数将只释放这个项，而不是它的子元素/valuestring。

cJSON_StringIsConst:这意味着字符串指向常量字符串。这意味着cJSON_Delete和其他函数不会尝试释放字符串。

**3、使用cJSON结构体**

对于每个值类型，都有一个cJSON_Create…可用于创建该类型项的函数。所有这些都将分配一个cJSON结构，稍后可以用cJSON_Delete删除它。请注意，您必须在某些时候删除它们，否则将导致内存泄漏。

重要提示:如果你已经添加了一个项到一个数组或一个对象，你不能删除它与cJSON_Delete。将它添加到数组或对象中会转移它的所有权，这样当数组或对象被删除时，它也会被删除。您还可以使用cJSON_SetValuestring来更改cJSON_String的valuestring，而不需要手动释放前面的valuestring。

（1）基础类型

使用cJSON_CreateNull创建null

booleans是用cJSON_CreateTrue、cJSON_CreateFalse或cJSON_CreateBool创建的

使用cJSON_CreateNumber创建数字。这将同时设置valuedouble和valueint。如果数值超出了整数的范围，则valueint使用INT_MAX或INT_MIN

字符串是用cJSON_CreateString(复制字符串)或cJSON_CreateStringReference(直接指向字符串)创建的。这意味着valuestring不会被cJSON_Delete删除，你负责它的生命周期，对常量很有用)

（2）数组

您可以使用cJSON_CreateArray创建一个空数组。cJSON_CreateArrayReference可以用来创建一个不“拥有”其内容的数组，因此它的内容不会被cJSON_Delete删除。

要向数组中添加项，请使用cJSON_AddItemToArray将项追加到末尾。使用cJSON_AddItemReferenceToArray，可以添加一个元素作为对另一个项、数组或字符串的引用。这意味着cJSON_Delete不会删除该项目的子属性或valuestring属性，因此如果它们已经在其他地方使用，则不会发生双重释放。要在中间插入项，请使用cJSON_InsertItemInArray。它将在给定的基于0的索引处插入一个项，并将所有现有项向右移动。

如果您想从给定索引的数组中取出一个项并继续使用它，请使用cJSON_DetachItemFromArray，它将返回分离的项，所以一定要将它分配给一个指针，否则将出现内存泄漏。

删除项目是使用cJSON_DeleteItemFromArray完成的。它的工作方式类似于cJSON_DetachItemFromArray，但是通过cJSON_Delete删除分离的项。

还可以就地替换数组中的项。不管是使用索引的cJSON_ReplaceItemInArray还是给cJSON_ReplaceItemViaPointer一个指向元素的指针。如果失败，cJSON_ReplaceItemViaPointer将返回0。这在内部所做的是分离旧的项，删除它并在其位置插入新项。

要获得数组的大小，请使用cJSON_GetArraySize。使用cJSON_GetArrayItem获得给定索引处的元素。

因为数组是作为链表存储的，所以通过索引对其进行迭代效率很低(O(n检索))，所以您可以使用cJSON_ArrayForEach宏在O(n)时间复杂度的情况下对数组进行迭代。

（3）对象

您可以使用cJSON_CreateObject创建一个空对象。cJSON_CreateObjectReference可以用来创建一个不“拥有”其内容的对象，因此它的内容不会被cJSON_Delete删除。

要向对象添加项，请使用cJSON_AddItemToObject。使用cJSON_AddItemToObjectCS向名称为常量或引用(项目的键，cJSON结构中的字符串)的对象添加一个项目，这样它就不会被cJSON_Delete释放。使用cJSON_AddItemReferenceToArray，可以添加一个元素作为对另一个对象、数组或字符串的引用。这意味着cJSON_Delete不会删除该项目的子属性或valuestring属性，因此如果它们已经在其他地方使用，则不会发生双重释放。

如果你想从一个对象中取出一个项，使用cJSON_DetachItemFromObjectCaseSensitive，它会返回分离的项，所以一定要把它分配给一个指针，否则你会有一个内存泄漏。

删除项目是通过cJSON_DeleteItemFromObjectCaseSensitive完成的。它的工作方式类似于cJSON_DetachItemFromObjectCaseSensitive，然后是cJSON_Delete。

还可以在适当的位置替换对象中的项。cJSON_ReplaceItemInObjectCaseSensitive使用键或cJSON_ReplaceItemViaPointer给予一个元素指针。如果失败，cJSON_ReplaceItemViaPointer将返回0。这在内部所做的是分离旧的项，删除它并在其位置插入新项。

要获得对象的大小，可以使用cJSON_GetArraySize，这样可以工作，因为在内部对象被存储为数组。

如果您想访问一个对象中的一个项，请使用cJSON_GetObjectItemCaseSensitive。

要遍历一个对象，可以使用cJSON_ArrayForEach宏，方法与使用数组相同。

cJSON还提供了方便的帮助函数，可以快速创建新项并将其添加到对象中，比如cJSON_AddNullToObject。它们返回一个指向新项的指针，如果失败则返回NULL。

4、解析JSON

给定一些以零结尾的字符串的JSON，您可以使用cJSON_Parse解析它。

cJSON *json = cJSON_Parse(string);

给定字符串中的一些JSON(无论是否以零结尾)，您可以使用cJSON_ParseWithLength对其进行解析。

cJSON *json = cJSON_ParseWithLength(string, buffer_length);

它将解析JSON并分配表示该JSON的cJSON项树。一旦它返回，您将完全负责在使用cJSON_Delete后释放它。

cJSON_Parse使用的分配器默认是malloc和free，但是可以用cJSON_InitHooks(全局地)修改。

如果发生错误，可以使用cJSON_GetErrorPtr访问指向输入字符串中错误位置的指针。注意，这可能会在多线程场景中产生竞争条件，在这种情况下，最好使用cJSON_ParseWithOpts和return_parse_end。默认情况下，在已解析的JSON后面的输入字符串中的字符将不会被视为错误。

如果你想要更多的选项，使用cJSON_ParseWithOpts(const char *value, const char **return_parse_end, cJSON_bool requirere_null_terminated)。return_parse_end返回一个指针，指向输入字符串中JSON的末尾或错误发生的位置(从而以线程安全的方式替换cJSON_GetErrorPtr)。require_null_terminated，如果设置为1，如果输入字符串包含JSON后面的数据，就会出现错误。

如果你想要更多的缓冲区长度选项，使用cJSON_ParseWithLengthOpts(const char *value, size_t buffer_length, const char **return_parse_end, cJSON_bool requirere_null_terminated)。

5、格式化打印JSON

给定一个cJSON条目树，您可以使用cJSON_Print将它们打印为字符串。

char *string = cJSON_Print(json);

它将分配一个字符串，并将树的JSON表示形式打印到其中。一旦它返回，您就完全有责任在使用分配器后释放它。(通常是免费的，这取决于cJSON_InitHooks设置的内容)。

cJSON_Print将打印带有空格的格式。如果希望不格式化打印，请使用cJSON_PrintUnformatted。

如果你有一个大致的想法，你的结果字符串将是多大，你可以使用cJSON_PrintBuffered(const cJSON *item, int prebuffer, cJSON_bool fmt)。fmt是一个布尔值，用于打开和关闭空白格式。prebuffer指定用于打印的第一个缓冲区大小。cJSON_Print目前使用256字节作为它的第一个缓冲区大小。一旦打印耗尽了空间，就会分配一个新的缓冲区，并在继续打印之前复制旧缓冲区。

使用cjson_printpreallocation (cJSON *item, char*buffer, const int length, const cJSON_bool format)可以完全避免这些动态的缓冲区分配。它需要一个缓冲来打印指针和它的长度。如果达到了长度，打印将失败并返回0。如果成功，则返回1。请注意，您应该提供比实际需要多5字节的内存，因为cJSON在估计所提供的内存是否足够方面不是100%准确的。

6、示例

在这个例子中，我们想要构建和解析以下JSON:

```c
{
"name": "Awesome 4K",

"resolutions": [

    {

        "width": 1280,

        "height": 720

    },

    {

        "width": 1920,

        "height": 1080

    },

    {

        "width": 3840,

        "height": 2160

    }

]
}
```

（1）打印

让我们构建上面的JSON并打印成字符串:

//创建带有支持分辨率列表的监视器

//注意:返回一个堆分配的字符串，使用后需要释放它。

```c
char *create_monitor(void)
{
    const unsigned int resolution_numbers[3][2] = {

        {1280, 720},

        {1920, 1080},

        {3840, 2160}

    };

    char *string = NULL;

    cJSON *name = NULL;

    cJSON *resolutions = NULL;

    cJSON *resolution = NULL;

    cJSON *width = NULL;

    cJSON *height = NULL;

    size_t index = 0;

    cJSON *monitor = cJSON_CreateObject();

    if (monitor == NULL)

    {

        goto end;

    }

    name = cJSON_CreateString("Awesome 4K");

    if (name == NULL)

    {

        goto end;

    }

    /* after creation was successful, immediately add it to the monitor,

    * thereby transferring ownership of the pointer to it */

    cJSON_AddItemToObject(monitor, "name", name);

    resolutions = cJSON_CreateArray();

    if (resolutions == NULL)

    {

        goto end;

    }

    cJSON_AddItemToObject(monitor, "resolutions", resolutions);

    for (index = 0; index < (sizeof(resolution_numbers) / (2 * sizeof(int))); ++index)

    {

        resolution = cJSON_CreateObject();

        if (resolution == NULL)

        {

            goto end;

        }

        cJSON_AddItemToArray(resolutions, resolution);

        width = cJSON_CreateNumber(resolution_numbers[index][0]);

        if (width == NULL)

        {

            goto end;

        }

        cJSON_AddItemToObject(resolution, "width", width);

        height = cJSON_CreateNumber(resolution_numbers[index][1]);

        if (height == NULL)

        {

            goto end;

        }

        cJSON_AddItemToObject(resolution, "height", height);

    }

    string = cJSON_Print(monitor);

    if (string == NULL)

    {

        fprintf(stderr, "Failed to print monitor.\n");

    }
    end:
    cJSON_Delete(monitor);

    return string;
}
```

或者，我们可以使用cJSON_Add…ToObject帮助函数，使我们的生活更容易:

//注意:返回一个堆分配的字符串，使用后需要释放它。

```c
char *create_monitor_with_helpers(void)
{
    const unsigned int resolution_numbers[3][2] = {

        {1280, 720},

        {1920, 1080},

        {3840, 2160}

    };

    char *string = NULL;

    cJSON *resolutions = NULL;

    size_t index = 0;

    cJSON *monitor = cJSON_CreateObject();

    if (cJSON_AddStringToObject(monitor, "name", "Awesome 4K") == NULL)

    {

        goto end;

    }

    resolutions = cJSON_AddArrayToObject(monitor, "resolutions");

    if (resolutions == NULL)

    {

        goto end;

    }

    for (index = 0; index < (sizeof(resolution_numbers) / (2 * sizeof(int))); ++index)

    {

        cJSON *resolution = cJSON_CreateObject();

        if (cJSON_AddNumberToObject(resolution, "width", resolution_numbers[index][0]) == NULL)

        {

            goto end;

        }

        if (cJSON_AddNumberToObject(resolution, "height", resolution_numbers[index][1]) == NULL)

        {

            goto end;

        }

        cJSON_AddItemToArray(resolutions, resolution);

    }

    string = cJSON_Print(monitor);

    if (string == NULL)

    {

        fprintf(stderr, "Failed to print monitor.\n");

    }
    end:
    cJSON_Delete(monitor);

    return string;
}
```

（2）解析

在这个例子中，我们将解析一个JSON在上述格式，并检查监视器是否支持全高清分辨率，同时打印一些诊断输出:

```c
/* return 1 if the monitor supports full hd, 0 otherwise */

int supports_full_hd(const char * const monitor)
{
    const cJSON *resolution = NULL;

    const cJSON *resolutions = NULL;

    const cJSON *name = NULL;

    int status = 0;

    cJSON *monitor_json = cJSON_Parse(monitor);

    if (monitor_json == NULL)

    {

        const char *error_ptr = cJSON_GetErrorPtr();

        if (error_ptr != NULL)

        {

            fprintf(stderr, "Error before: %s\n", error_ptr);

        }

        status = 0;

        goto end;

    }

    name = cJSON_GetObjectItemCaseSensitive(monitor_json, "name");

    if (cJSON_IsString(name) && (name->valuestring != NULL))

    {

        printf("Checking monitor \"%s\"\n", name->valuestring);

    }

    resolutions = cJSON_GetObjectItemCaseSensitive(monitor_json, "resolutions");

    cJSON_ArrayForEach(resolution, resolutions)

    {

        cJSON *width = cJSON_GetObjectItemCaseSensitive(resolution, "width");

        cJSON *height = cJSON_GetObjectItemCaseSensitive(resolution, "height");

        if (!cJSON_IsNumber(width) || !cJSON_IsNumber(height))

        {

            status = 0;

            goto end;

        }

        if ((width->valuedouble == 1920) && (height->valuedouble == 1080))

        {

            status = 1;

            goto end;

        }

    }
    end:
    cJSON_Delete(monitor_json);

    return status;
}
```

注意，除了cJSON_Parse的结果之外，没有空检查，因为cJSON_GetObjectItemCaseSensitive检查NULL输入，所以NULL值只传播，如果输入为空，cJSON_IsNumber和cJSON_IsString返回0。

**注意事项**

cJSON不支持包含零字符’\0’或\u0000的字符串。这对于当前的API是不可能的，因为字符串以0结尾。

cJSON只支持UTF-8编码的输入。但是在大多数情况下，它不会拒绝无效的UTF-8作为输入，它只是按原样传播它。只要输入不包含无效的UTF-8，输出将始终是有效的UTF-8。

cJSON是用ANSI C(或C89, C90)编写的。如果编译器或C库不遵循此标准，则不能保证正确的行为。

注意:ansic不是c++，因此不应该用c++编译器编译。但是，您可以用C编译器编译它，并将它与您的c++代码链接起来。虽然使用c++编译器进行编译可以工作，但不能保证正确的行为。

除了IEEE754双精度浮点数之外，cJSON没有正式支持任何双实现。它可能仍然与其他实现一起工作，但这些实现的错误将被认为是无效的。

cJSON支持的浮点文字的最大长度目前是63个字符。

cJSON不支持嵌套太深的数组和对象，因为这会导致堆栈溢出。为了防止这种情况发生，cJSON将深度限制为CJSON_NESTING_LIMIT，默认为1000，但可以在编译时更改。

通常cJSON不是线程安全的。

但是在以下条件下是线程安全的:

cJSON_GetErrorPtr永远不会被使用(可以使用cJSON_ParseWithOpts的return_parse_end参数)

cJSON_InitHooks只会在任何线程中使用cJSON之前被调用。

在所有对cJSON函数的调用都返回之前，不会调用setlocale。

最初创建cJSON时，它没有遵循JSON标准，也没有区分大写字母和小写字母。如果您想要正确的、符合标准的行为，您需要在可用的情况下使用与案例相关的功能。

cJSON支持解析和打印包含具有多个同名成员的对象的JSON。但是cJSON_GetObjectItemCaseSensitive总是只返回第一个。
