-- Upgrade: 1.5.0 -> 1.5.1

DO $$
BEGIN
   ALTER OPERATOR FAMILY spoint USING gist ADD
      OPERATOR 17 <-> (spoint, spoint) FOR ORDER BY float_ops;
EXCEPTION
   WHEN duplicate_object THEN NULL;
   WHEN OTHERS THEN RAISE;
END;
$$;
