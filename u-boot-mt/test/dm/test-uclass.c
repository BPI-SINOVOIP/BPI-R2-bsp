/*
 * Copyright (c) 2013 Google, Inc
 *
 * (C) Copyright 2012
 * Pavel Herrmann <morpheus.ibis@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <malloc.h>
#include <dm.h>
#include <errno.h>
#include <dm/test.h>
#include <dm/ut.h>
#include <asm/io.h>
#include <linux/list.h>

static struct dm_test_state *dms = &global_test_state;

int test_ping(struct device *dev, int pingval, int *pingret)
{
	const struct test_ops *ops = device_get_ops(dev);

	if (!ops->ping)
		return -ENOSYS;

	return ops->ping(dev, pingval, pingret);
}

static int test_post_bind(struct device *dev)
{
	dm_testdrv_op_count[DM_TEST_OP_POST_BIND]++;

	return 0;
}

static int test_pre_unbind(struct device *dev)
{
	dm_testdrv_op_count[DM_TEST_OP_PRE_UNBIND]++;

	return 0;
}

static int test_post_probe(struct device *dev)
{
	struct device *prev = list_entry(dev->uclass_node.prev, struct device,
					 uclass_node);
	struct dm_test_uclass_perdev_priv *priv = dev->uclass_priv;
	struct uclass *uc = dev->uclass;

	dm_testdrv_op_count[DM_TEST_OP_POST_PROBE]++;
	ut_assert(priv);
	ut_assert(device_active(dev));
	priv->base_add = 0;
	if (dms->skip_post_probe)
		return 0;
	if (&prev->uclass_node != &uc->dev_head) {
		struct dm_test_uclass_perdev_priv *prev_uc_priv
				= prev->uclass_priv;
		struct dm_test_pdata *pdata = prev->platdata;

		ut_assert(pdata);
		ut_assert(prev_uc_priv);
		priv->base_add = prev_uc_priv->base_add + pdata->ping_add;
	}

	return 0;
}

static int test_pre_remove(struct device *dev)
{
	dm_testdrv_op_count[DM_TEST_OP_PRE_REMOVE]++;

	return 0;
}

static int test_init(struct uclass *uc)
{
	dm_testdrv_op_count[DM_TEST_OP_INIT]++;
	ut_assert(uc->priv);

	return 0;
}

static int test_destroy(struct uclass *uc)
{
	dm_testdrv_op_count[DM_TEST_OP_DESTROY]++;

	return 0;
}

UCLASS_DRIVER(test) = {
	.name		= "test",
	.id		= UCLASS_TEST,
	.post_bind	= test_post_bind,
	.pre_unbind	= test_pre_unbind,
	.post_probe	= test_post_probe,
	.pre_remove	= test_pre_remove,
	.init		= test_init,
	.destroy	= test_destroy,
	.priv_auto_alloc_size	= sizeof(struct dm_test_uclass_priv),
	.per_device_auto_alloc_size = sizeof(struct dm_test_uclass_perdev_priv),
};
