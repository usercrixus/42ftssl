#include "parse.h"
#include <stddef.h>
#include <string.h>
#include "../../42libft/ft_base/libft.h"

t_option parse_cmd(const char *cmd)
{
	if (!cmd)
		return NONE;
	if (ft_strncmp(cmd, "md5", ft_strlen("md5") + 1) == 0)
		return MD5;
	if (ft_strncmp(cmd, "sha256", ft_strlen("sha256") + 1) == 0)
		return SHA256;
	return NONE;
}

/* add unique flag chars from token (like "-pqs"), capture -s VALUE if present */
static void absorb_flag_group(const char *arg, char *flags_buff, char **s_arg, int argc, char **argv, int *i)
{
	const char *p = arg + 1; /* skip '-' */
	while (*p)
	{
		if (!ft_strchr(flags_buff, *p))
		{
			size_t flags_buff_len = ft_strlen(flags_buff);
			if (flags_buff_len + 1 < 64)
			{
				flags_buff[flags_buff_len] = *p;
				flags_buff[flags_buff_len + 1] = '\0';
			}
		}
		if (*p == 's')
		{
			if (*s_arg == NULL && *i + 1 < argc && argv[*i + 1][0] != '-')
			{
				*s_arg = argv[*i + 1];
				(*i)++; /* skip value */
			}
		}
		p++;
	}
}

/* parse argv into t_cmd_data (flags, -s value, file, and trailing errors) */
void parse_data(int argc, char **argv, t_cmd_data *data)
{
	static char flags_buff[64];
	int i = 2;
	ft_memset(data, 0, sizeof(*data));
	flags_buff[0] = '\0';
	if (argc > 2 && argv[2][0] == '-')
	{
		while (i < argc && argv[i][0] == '-')
		{
			absorb_flag_group(argv[i], flags_buff, &data->s_arg, argc, argv, &i);
			i++;
		}
		if (flags_buff[0])
			data->flags = flags_buff;
		if (i < argc && argv[i][0] != '-')
			data->file_arg = argv[i++];
		for (int j = i; j < argc && data->errc < (int)(sizeof(data->errv) / sizeof(data->errv[0])); ++j)
			data->errv[data->errc++] = argv[j];
	}
	else if (argc > 2)
	{
		data->file_arg = argv[2];
		for (int j = 3; j < argc && data->errc < (int)(sizeof(data->errv) / sizeof(data->errv[0])); ++j)
			data->errv[data->errc++] = argv[j];
	}
}

bool isFlagSet(const char *flags, char c)
{
	if (!flags)
		return false;
	for (int i = 0; flags[i]; ++i)
		if (flags[i] == c)
			return true;
	return false;
}
