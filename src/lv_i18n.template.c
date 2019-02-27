#include "./lv_i18n.h"

/*SAMPLE_START*/

////////////////////////////////////////////////////////////////////////////////
// Define plural operands
// http://unicode.org/reports/tr35/tr35-numbers.html#Operands

// Integer version, simplified

static uint32_t __attribute__((unused)) op_n(int32_t val)
{
    return (uint32_t)(val < 0 ? -val : val);
}

static uint32_t __attribute__((unused)) op_i(uint32_t val)
{
    return val;
}

// always zero, when decimal part not exists.
static uint32_t __attribute__((unused)) op_v(uint32_t val __attribute__((unused)))
{
    return 0;
}
static uint32_t __attribute__((unused)) op_w(uint32_t val __attribute__((unused)))
{
    return 0;
}
static uint32_t __attribute__((unused)) op_f(uint32_t val __attribute__((unused)))
{
    return 0;
}
static uint32_t __attribute__((unused)) op_t(uint32_t val __attribute__((unused)))
{
    return 0;
}




static lv_i18n_phrase_t en_gb_singulars[] = {
    {"s_en_only", "english only"},
    {"s_translated", "s translated"},
    {NULL, NULL} // End mark
};



static lv_i18n_phrase_t en_gb_plurals_one[] = {
    {"p_i_have_dogs", "I have %d dog"},
    {NULL, NULL} // End mark
};


static lv_i18n_phrase_t en_gb_plurals_other[] = {
    {"p_i_have_dogs", "I have %d dogs"},
    {NULL, NULL} // End mark
};



static uint8_t en_gb_plural_fn(int32_t num)
{
    uint32_t n = op_n(num);
    uint32_t i = op_i(n);
    uint32_t v = op_v(n);

    if ((i == 1 && v == 0)) return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}


static const lv_i18n_lang_t en_gb_lang = {
    .locale_name = "en-GB",
    .singulars = en_gb_singulars,
    .plurals[LV_I18N_PLURAL_TYPE_ONE] = en_gb_plurals_one,
    .plurals[LV_I18N_PLURAL_TYPE_OTHER] = en_gb_plurals_other,
    .locale_plural_fn = en_gb_plural_fn
};



static lv_i18n_phrase_t ru_ru_singulars[] = {
    {"s_translated", "s переведено"},
    {NULL, NULL} // End mark
};



static lv_i18n_phrase_t ru_ru_plurals_one[] = {
    {"p_i_have_dogs", "У меня %d собакен"},
    {NULL, NULL} // End mark
};


static lv_i18n_phrase_t ru_ru_plurals_few[] = {
    {"p_i_have_dogs", "У меня %d собакена"},
    {NULL, NULL} // End mark
};


static lv_i18n_phrase_t ru_ru_plurals_many[] = {
    {"p_i_have_dogs", "У меня %d собакенов"},
    {NULL, NULL} // End mark
};



static uint8_t ru_ru_plural_fn(int32_t num)
{
    uint32_t n = op_n(num);
    uint32_t v = op_v(n);
    uint32_t i = op_i(n);
    uint32_t i10 = i % 10;
    uint32_t i100 = i % 100;
    if ((v == 0 && i10 == 1 && i100 != 11)) return LV_I18N_PLURAL_TYPE_ONE;
    if ((v == 0 && (2 <= i10 && i10 <= 4) && (!(12 <= i100 && i100 <= 14)))) return LV_I18N_PLURAL_TYPE_FEW;
    if ((v == 0 && i10 == 0) || (v == 0 && (5 <= i10 && i10 <= 9)) || (v == 0 && (11 <= i100 && i100 <= 14))) return LV_I18N_PLURAL_TYPE_MANY;
    return LV_I18N_PLURAL_TYPE_OTHER;
}


static const lv_i18n_lang_t ru_ru_lang = {
    .locale_name = "ru-RU",
    .singulars = ru_ru_singulars,
    .plurals[LV_I18N_PLURAL_TYPE_ONE] = ru_ru_plurals_one,
    .plurals[LV_I18N_PLURAL_TYPE_FEW] = ru_ru_plurals_few,
    .plurals[LV_I18N_PLURAL_TYPE_MANY] = ru_ru_plurals_many,
    .locale_plural_fn = ru_ru_plural_fn
};







static uint8_t de_de_plural_fn(int32_t num)
{
    uint32_t n = op_n(num);
    uint32_t i = op_i(n);
    uint32_t v = op_v(n);

    if ((i == 1 && v == 0)) return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}


static const lv_i18n_lang_t de_de_lang = {
    .locale_name = "de-DE",


    .locale_plural_fn = de_de_plural_fn
};


const lv_i18n_lang_pack_t lv_i18n_language_pack[] = {
    &en_gb_lang,
    &ru_ru_lang,
    &de_de_lang,
    NULL // End mark
};

/*SAMPLE_END*/


// Internal state
static const lv_i18n_lang_pack_t * current_lang_pack;
static const lv_i18n_lang_t * current_lang;


/**
 * Reset internal state. For testing.
 */
void __lv_i18n_reset(void)
{
    current_lang_pack = NULL;
    current_lang = NULL;
}

/**
 * Set the languages for internationalization
 * @param langs pointer to the array of languages. (Last element has to be `NULL`)
 */
int lv_i18n_init(const lv_i18n_lang_pack_t * langs)
{
    if(langs == NULL) return -1;
    if(langs[0] == NULL) return -1;

    current_lang_pack = langs;
    current_lang = langs[0];     /*Automatically select the first language*/
    return 0;
}

/**
 * Change the localization (language)
 * @param l_name name of the translation locale to use. E.g. "en-GB"
 */
int lv_i18n_set_locale(const char * l_name)
{
    if(current_lang_pack == NULL) return -1;

    uint16_t i;

    for(i = 0; current_lang_pack[i] != NULL; i++) {
        // Found -> finish
        if(strcmp(current_lang_pack[i]->locale_name, l_name) == 0) {
            current_lang = current_lang_pack[i];
            return 0;
        }
    }

    return -1;
}


const char * __lv_i18n_get_text_core(lv_i18n_phrase_t * trans, const char * msg_id)
{
    uint16_t i;
    for(i = 0; trans[i].msg_id != NULL; i++) {
        if(strcmp(trans[i].msg_id, msg_id) == 0) {
            /*The msg_id has found. Check the translation*/
            if(trans[i].translation) return trans[i].translation;
        }
    }

    return NULL;
}


/**
 * Get the translation from a message ID
 * @param msg_id message ID
 * @return the translation of `msg_id` on the set local
 */
const char * lv_i18n_get_text(const char * msg_id)
{
    if(current_lang == NULL) return msg_id;

    const lv_i18n_lang_t * lang = current_lang;

    if(lang->singulars == NULL) {
        if(lang == current_lang_pack[0]) return msg_id;
        else lang = current_lang_pack[0];

        if(lang->singulars == NULL) return msg_id;
    }

    /*Find the translation*/
    const void * txt = __lv_i18n_get_text_core(lang->singulars, msg_id);
    if(txt == NULL) {
        if(lang == current_lang_pack[0]) return msg_id;
        else lang = current_lang_pack[0];
    }

    /*Try again with the default language*/
    if(lang->singulars == NULL) return msg_id;

    txt = __lv_i18n_get_text_core(lang->singulars, msg_id);
    if(txt == NULL) return msg_id;

    return txt;
}

/**
 * Get the translation from a message ID and apply the language's plural rule to get correct form
 * @param msg_id message ID
 * @param num an integer to select the correct plural form
 * @return the translation of `msg_id` on the set local
 */
const char * lv_i18n_get_text_plural(const char * msg_id, int32_t num)
{
    if(current_lang == NULL) return msg_id;

    const lv_i18n_lang_t * lang = current_lang;
    if(lang->plurals == NULL || lang->locale_plural_fn == NULL) {
        if(lang == current_lang_pack[0]) return msg_id;
        else lang = current_lang_pack[0];

        if(lang->plurals == NULL) return msg_id;
    }

    lv_i18n_plural_type_t ptype = lang->locale_plural_fn(num);

    if(lang->plurals[ptype] == NULL) {
        if(lang == current_lang_pack[0]) return msg_id;
        else lang = current_lang_pack[0];
    }

    /*Find the translation*/
    const void * txt = __lv_i18n_get_text_core(lang->plurals[ptype], msg_id);
    if(txt == NULL) {
        if(lang == current_lang_pack[0]) return msg_id;
        else lang = current_lang_pack[0];
    }

    /*Try again with the default language*/
    if(lang->plurals == NULL || lang->locale_plural_fn == NULL) return msg_id;

    ptype = lang->locale_plural_fn(num);

    if(lang->plurals[ptype] == NULL) return msg_id;

    txt = __lv_i18n_get_text_core(lang->plurals[ptype], msg_id);

    if(txt == NULL) return msg_id;

    return txt;
}

/**
 * Get the name of the currently used locale.
 * @return name of the currently used locale. E.g. "en-GB"
 */
const char * lv_i18n_get_current_locale(void)
{
    if(!current_lang) return NULL;
    return current_lang->locale_name;
}
